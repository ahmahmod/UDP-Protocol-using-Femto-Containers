/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "femtocontainer/femtocontainer.h"
#include "femtocontainer/builtin_shared.h"
#include "femtocontainer/builtin_calls.h"
#include "femtocontainer/instruction.h"
#include "femtocontainer/config.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#if ENABLE_DEBUG
#include "xtimer.h"
#include <stdio.h>
#endif

typedef int dont_be_pedantic;

static int _check_mem(const f12r_t *femtoc, uint8_t size, const intptr_t addr, uint8_t type)
{
    const intptr_t end = addr + size;
    for (const f12r_mem_region_t *region = &femtoc->stack_region; region; region = region->next) {
        if ((addr >= (intptr_t)(region->start)) &&
                (end <= (intptr_t)(region->start + region->len)) &&
                (region->flag & type)) {

            return 0;
        }
    }

    return -1;
}

static inline int _check_load(const f12r_t *femtoc, uint8_t size, const intptr_t addr)
{
    return _check_mem(femtoc, size, addr, FC_MEM_REGION_READ);
}

static inline int _check_store(const f12r_t *femtoc, uint8_t size, const intptr_t addr)
{
    return _check_mem(femtoc, size, addr, FC_MEM_REGION_WRITE);
}

int f12r_store_allowed(const f12r_t *femtoc, void *addr, size_t size)
{
    return _check_store(femtoc, size, (intptr_t)addr);
}

int f12r_load_allowed(const f12r_t *femtoc, void *addr, size_t size)
{
    return _check_load(femtoc, size, (intptr_t)addr);
}

static f12r_call_t _femtoc_get_call(uint32_t num)
{
    switch(num) {
        default:
            return f12r_get_external_call(num);
    }
}

/**
 * This is a set of macros to easily implement the similar eBPF instructions
 */

/* Macro for the destination, source and immediate value */
#define DST regmap[instr->dst] /* DST is the register targeted by the instruction */
#define SRC regmap[instr->src] /* SRC is the source register from the instruction */
#define IMM instr->immediate   /* And this one matches the immediate value in the instruction */

/* Two macros that jump to the start of the instruction pipeline. */
#define CONT       { goto select_instr; } /* Continue execution with the next one */
#define CONT_JUMP  { goto jump_instr; } /* Execute the jump and continue */

/* Check if we implement 32 bit instructions */
#if (FEMTO_CONTAINER_ENABLE_ALU32)

/* Generate the ALU instruction, based on the opcode name and the operation
 * itself. ALU(ADD, +) generates the 2 or 4 instructions implementing the add
 * instruction, using '+' in C. Generates both the DST += SRC and DST += IMM */
#define ALU(OPCODE, OP)         \
    ALU64_##OPCODE##_REG:         \
        DST = DST OP SRC;       \
        CONT;                   \
    ALU64_##OPCODE##_IMM:       \
        DST = DST OP IMM;       \
        CONT;                   \
    ALU32_##OPCODE##_REG:         \
        DST = (uint32_t) DST OP (uint32_t) SRC;   \
        CONT;                   \
    ALU32_##OPCODE##_IMM:           \
        DST = (uint32_t) DST OP (uint32_t) IMM;   \
        CONT;
#else
#define ALU(OPCODE, OP)         \
    ALU64_##OPCODE##_REG:         \
        DST = DST OP SRC;       \
        CONT;                   \
    ALU64_##OPCODE##_IMM:       \
        DST = DST OP IMM;       \
        CONT;
#endif

/* Generate jump type instructions, similar to the ALU instructions */
#define COND_JMP(SIGN, OPCODE, CMP_OP)              \
    JMP_##OPCODE##_REG:                  \
        jump_cond = (SIGN##nt64_t) DST CMP_OP (SIGN##nt64_t)SRC; \
        CONT_JUMP;                           \
    JMP_##OPCODE##_IMM:                 \
        jump_cond = (SIGN##nt64_t) DST CMP_OP (SIGN##nt64_t)IMM; \
        CONT_JUMP;                           \

/* This generates values for the jumptable array. It combines the same opcode
 * name as used in the code generation macros and the numeric opcode value to
 * create a jumptable entry. Entries are generated for 64 and 32 bit types */
#if FEMTO_CONTAINER_ENABLE_ALU32
#define ALU_OPCODE_REG(OPCODE, VALUE) \
    [VALUE | 0x0C ] = &&ALU32_##OPCODE##_REG, \
    [VALUE | 0x0F ] = &&ALU64_##OPCODE##_REG

#define ALU_OPCODE_IMM(OPCODE, VALUE)   \
    [VALUE | 0x04 ] = &&ALU32_##OPCODE##_IMM, \
    [VALUE | 0x07 ] = &&ALU64_##OPCODE##_IMM
#else
#define ALU_OPCODE_REG(OPCODE, VALUE) \
    [VALUE | 0x0F ] = &&ALU64_##OPCODE##_REG

#define ALU_OPCODE_IMM(OPCODE, VALUE)   \
    [VALUE | 0x07 ] = &&ALU64_##OPCODE##_IMM
#endif

/* And this macro generates a register and immediate type jumptable entry based
 * on the above macros */
#define ALU_OPCODE(OPCODE, VALUE) \
    ALU_OPCODE_REG(OPCODE, VALUE), \
    ALU_OPCODE_IMM(OPCODE, VALUE)

/* Jump-instruction specific jumptable generator, includes immediate and
 * register based code */
#define JMP_OPCODE(OPCODE, VALUE) \
    [VALUE | 0x05] = &&JMP_##OPCODE##_IMM, \
    [VALUE | 0x0D] = &&JMP_##OPCODE##_REG

/* And finaly this generates the opcode entries for memory instructions. It
 * generates the full set of size types supported by eBPF instructions */
#define MEM_OPCODE(OPCODE, VALUE) \
    [VALUE | 0x10] = &&MEM_##OPCODE##_BYTE, \
    [VALUE | 0x08] = &&MEM_##OPCODE##_HALF, \
    [VALUE | 0x00] = &&MEM_##OPCODE##_WORD, \
    [VALUE | 0x18] = &&MEM_##OPCODE##_LONG \

int f12r_run(f12r_t *femtoc, const void *ctx, int64_t *result)
{
    int res = FC_OK;
    femtoc->branches_remaining = FEMTO_CONTAINER_BRANCHES_ALLOWED;
    uint64_t regmap[11] = { 0 };
    regmap[1] = (uint64_t)(uintptr_t)ctx;
    regmap[10] = (uint64_t)(uintptr_t)(femtoc->stack + femtoc->stack_size);


    const bpf_instruction_t *instr = (const bpf_instruction_t*)f12r_text(femtoc);
    bool jump_cond = false;

#if ENABLE_DEBUG
    uint32_t start = xtimer_now_usec();
#endif

    res = f12r_verify_preflight(femtoc);
    if (res < 0) {
        return res;
    }

#if ENABLE_DEBUG
    uint32_t mid = xtimer_now_usec();
#endif

    /* Create an instruction jumptable with calculated addresses for the goto */
    static const void * const _jumptable[256] = {
        [0 ... 255] = &&invalid_instruction,
        ALU_OPCODE(ADD, 0x00),
        ALU_OPCODE(SUB, 0x10),
        ALU_OPCODE(MUL, 0x20),
        ALU_OPCODE(DIV, 0x30),
        ALU_OPCODE(OR,  0x40),
        ALU_OPCODE(AND, 0x50),
        ALU_OPCODE(LSH, 0x60),
        ALU_OPCODE(RSH, 0x70),
        ALU_OPCODE_REG(NEG, 0x80),
        ALU_OPCODE(MOD, 0x90),
        ALU_OPCODE(XOR, 0xa0),
        ALU_OPCODE(MOV, 0xb0),
        ALU_OPCODE(ARSH, 0xc0),

        [0x05] = &&JUMP_ALWAYS,
        JMP_OPCODE(EQ, 0x10),
        JMP_OPCODE(GT, 0x20),
        JMP_OPCODE(GE, 0x30),
        JMP_OPCODE(SET, 0x40),
        JMP_OPCODE(NE, 0x50),
        JMP_OPCODE(SGT, 0x60),
        JMP_OPCODE(SGE, 0x70),
        JMP_OPCODE(LT, 0xA0),
        JMP_OPCODE(LE, 0xB0),
        JMP_OPCODE(SLT, 0xC0),
        JMP_OPCODE(SLE, 0xD0),

        [0x18] = &&MEM_LDDW_IMM,
        [0xB8] = &&MEM_LDDWD_IMM,
        [0xD8] = &&MEM_LDDWR_IMM,

        MEM_OPCODE(STX, 0x63),
        MEM_OPCODE(ST,  0x62),
        MEM_OPCODE(LDX, 0x61),


        [0x85] = &&OPCODE_CALL,
        [0x95] = &&OPCODE_RETURN,
    };

    goto femtoc_start;

jump_instr:
    if (jump_cond) {
        instr += instr->offset;
        if ((!(femtoc->flags & FC_CONFIG_NO_RETURN)) &&
                femtoc->branches_remaining-- == 0) {
            res = FC_OUT_OF_BRANCHES;
            goto exit;
        }
    }

    /* Intentionally falls through to select_instr */
select_instr:
    instr++;
femtoc_start:
    //femtoc->instruction_count++;
    goto *_jumptable[instr->opcode];

/* Macros implementing the instruction code for the simple ALU based operations */
    ALU(ADD,  +)
    ALU(SUB,  -)
    ALU(AND,  &)
    ALU(OR,   |)
    ALU(LSH, <<)
    ALU(RSH, >>)
    ALU(XOR,  ^)
    ALU(MUL,  *)

ALU64_MOD_REG:
    if (SRC == 0) {
        res = FC_ILLEGAL_DIV;
        goto exit;
    }
    DST = DST % SRC;
    CONT;
ALU64_MOD_IMM:
    if (IMM == 0) {
        res = FC_ILLEGAL_DIV;
        goto exit;
    }
    DST = DST % IMM;
    CONT;
#if (FEMTO_CONTAINER_ENABLE_ALU32)
ALU32_MOD_REG:
    if (SRC == 0) {
        res = FC_ILLEGAL_DIV;
        goto exit;
    }
    DST = (uint32_t)DST % (uint32_t)SRC;
    CONT;
ALU32_MOD_IMM:
    if (IMM == 0) {
        res = FC_ILLEGAL_DIV;
        goto exit;
    }
    DST = (uint32_t)DST % (uint32_t)IMM;
    CONT;
#endif

ALU64_DIV_REG:
    if (SRC == 0) {
        res = FC_ILLEGAL_DIV;
        goto exit;
    }
    DST = DST / SRC;
    CONT;
ALU64_DIV_IMM:
    if (IMM == 0) {
        res = FC_ILLEGAL_DIV;
        goto exit;
    }
    DST = DST / IMM;
    CONT;
#if (FEMTO_CONTAINER_ENABLE_ALU32)
ALU32_DIV_REG:
    if (SRC == 0) {
        res = FC_ILLEGAL_DIV;
        goto exit;
    }
    DST = (uint32_t)DST / (uint32_t)SRC;
    CONT;
ALU32_DIV_IMM:
    if (IMM == 0) {
        res = FC_ILLEGAL_DIV;
        goto exit;
    }
    DST = (uint32_t)DST / (uint32_t)IMM;
    CONT;
#endif

ALU64_NEG_REG:
    DST = -(int64_t)DST;
    CONT;

#if (FEMTO_CONTAINER_ENABLE_ALU32)
ALU32_NEG_REG:
    DST = -(int32_t)DST;
    CONT;

    /* MOV */
ALU32_MOV_IMM:
    DST = (uint32_t)IMM;
    CONT;
ALU32_MOV_REG:
    DST = (uint32_t)SRC;
    CONT;
#endif
ALU64_MOV_IMM:
    DST = IMM;
    CONT;
ALU64_MOV_REG:
    DST = SRC;
    CONT;

    /* Arithmetic shift */
ALU64_ARSH_REG:
    (*(int64_t*) &DST) >>= SRC;
    CONT;
ALU64_ARSH_IMM:
    (*(int64_t*) &DST) >>= IMM;
    CONT;
#if (FEMTO_CONTAINER_ENABLE_ALU32)
ALU32_ARSH_REG:
    DST = (int32_t)DST >> SRC;
    CONT;
ALU32_ARSH_IMM:
    DST =  (int32_t)DST >> IMM;
    CONT;
#endif

MEM_LDDW_IMM:
    DST = (uint64_t)instr->immediate;
    DST |= ((uint64_t)((instr+1)->immediate)) << 32;
    instr++;
    CONT;

MEM_LDDWD_IMM:
    DST = (intptr_t)f12r_data(femtoc);
    DST += (uint64_t)instr->immediate;
    DST += ((uint64_t)((instr+1)->immediate)) << 32;
    instr++;
    CONT;

MEM_LDDWR_IMM:
    DST = (intptr_t)f12r_rodata(femtoc);
    DST += (uint64_t)instr->immediate;
    DST += ((uint64_t)((instr+1)->immediate)) << 32;
    instr++;
    CONT;

#define MEM(SIZEOP, SIZE)                     \
      MEM_STX_##SIZEOP:                       \
          if (_check_store(femtoc, sizeof(SIZE), DST + instr->offset) < 0) { \
              goto mem_error; \
          } \
          *(SIZE *)(uintptr_t)(DST + instr->offset) = SRC;   \
          CONT;                               \
      MEM_ST_##SIZEOP:                        \
          if (_check_store(femtoc, sizeof(SIZE), DST + instr->offset) < 0) { \
              goto mem_error; \
          } \
          *(SIZE *)(uintptr_t)(DST + instr->offset) = IMM;   \
          CONT;                               \
      MEM_LDX_##SIZEOP:                       \
          if (_check_load(femtoc, sizeof(SIZE), SRC + instr->offset) < 0) { \
              goto mem_error; \
          } \
          DST = *(const SIZE *)(uintptr_t)(SRC + instr->offset);   \
          CONT;

      MEM(BYTE, uint8_t)
      MEM(HALF, uint16_t)
      MEM(WORD, uint32_t)
      MEM(LONG, uint64_t)
#undef LDST


JUMP_ALWAYS:
    jump_cond = 1;
    CONT_JUMP;
    COND_JMP(ui, EQ, ==)
    COND_JMP(ui, GT, >)
    COND_JMP(ui, GE, >=)
    COND_JMP(ui, LT, <)
    COND_JMP(ui, LE, <=)
    COND_JMP(ui, SET, &)
    COND_JMP(ui, NE, !=)
    COND_JMP(i, SGT, >)
    COND_JMP(i, SGE, >=)
    COND_JMP(i, SLT, <)
    COND_JMP(i, SLE, <=)
OPCODE_CALL:
    {
        f12r_call_t call = _femtoc_get_call(instr->immediate);
        if (call) {
            regmap[0] = (*(call))(femtoc,
                                  regmap[1],
                                  regmap[2],
                                  regmap[3],
                                  regmap[4],
                                  regmap[5]);
            CONT;
        }
        else {
            res = FC_ILLEGAL_CALL;
            goto exit;
        }
    }
OPCODE_RETURN:
    goto exit;

invalid_instruction:
    res = FC_ILLEGAL_INSTRUCTION;
    goto exit;

mem_error:
    res = FC_ILLEGAL_MEM;

exit:

    *result = regmap[0];
#if ENABLE_DEBUG
    uint32_t end = xtimer_now_usec();

    printf("startup = %"PRIu32" us\n", mid - start);
    printf("timing = %"PRIu32" us\n", end - mid);
#endif
    return res;
}


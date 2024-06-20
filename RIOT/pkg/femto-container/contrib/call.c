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
#include <stdio.h>
#include <string.h>

#include "femtocontainer/femtocontainer.h"
#include "shared.h"
#include "femtocontainer/store.h"
#include "call.h"
#include "xtimer.h"

#ifdef MODULE_GCOAP
#include "net/gcoap.h"
#include "net/nanocoap.h"
#endif
#include "saul.h"
#include "saul_reg.h"
#include "fmt.h"

#ifdef MODULE_ZTIMER
#include "ztimer.h"
#endif

/*MALLOC PKTBUF*/
extern uint32_t bpf_gnrc_pktbuf_start_write_malloc(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
extern uint32_t bpf_gnrc_pktbuf_release_malloc(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
extern uint32_t bpf_gnrc_pktbuf_mark_malloc(f12r_t *bpf, uint32_t pkt, uint32_t size, uint32_t type, uint32_t a4, uint32_t a5);

/*STATIC PKTBUF*/
extern uint32_t bpf_gnrc_pktbuf_start_write_static(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
extern uint32_t bpf_gnrc_pktbuf_release_static(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
extern uint32_t bpf_gnrc_pktbuf_mark_static(f12r_t *bpf, uint32_t pkt, uint32_t size, uint32_t type, uint32_t a4, uint32_t a5);

/*PKT*/
extern uint32_t bpf_gnrc_pktsnip_search_type(f12r_t *bpf, uint32_t pkt, uint32_t type, uint32_t a3, uint32_t a4, uint32_t a5);

extern uint32_t bpf_gnrc_pktsnip_get_type(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
extern uint32_t bpf_gnrc_pktsnip_set_type(f12r_t *bpf, uint32_t pkt, uint32_t type, uint32_t a3, uint32_t a4, uint32_t a5);

extern uint32_t bpf_gnrc_pktsnip_get_next(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
extern uint32_t bpf_gnrc_pktsnip_set_next(f12r_t *bpf, uint32_t pkt, uint32_t next, uint32_t a3, uint32_t a4, uint32_t a5);

extern uint32_t bpf_gnrc_pktsnip_get_size(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

extern uint32_t bpf_gnrc_pktsnip_get_data(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

extern uint32_t bpf_gnrc_pktsnip_get_udp_hcsum(f12r_t *bpf, uint32_t udp_hdr, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
extern uint32_t bpf_gnrc_pktsnip_get_udp_dst_port(f12r_t *bpf, uint32_t udp_hdr, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
extern uint32_t bpf_gnrc_pktsnip_set_udp_len(f12r_t *bpf, uint32_t udp_hdr, uint32_t size, uint32_t a3, uint32_t a4, uint32_t a5);

extern uint32_t bpf_gnrc_get_pkt_len(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

/*NETAPI*/
extern uint32_t bpf_gnrc_netapi_dispatch_send(f12r_t *bpf, uint32_t pkt, uint32_t demux_ctx, uint32_t type, uint32_t a4, uint32_t a5);
extern uint32_t bpf_gnrc_netapi_dispatch_receive(f12r_t *bpf, uint32_t type, uint32_t demux_ctx, uint32_t pkt, uint32_t a4, uint32_t a5);

/*Cross Layer*/
extern uint32_t bpf_calc_csum(f12r_t *bpf, uint32_t hdr, uint32_t pseudo_hdr, uint32_t payload, uint32_t a4, uint32_t a5);

/*UDP*/
extern uint32_t bpf_gnrc_udp_calc_csum(f12r_t *bpf, uint32_t hdr, uint32_t pseudo_hdr, uint32_t a3, uint32_t a4, uint32_t a5);

/*--------------------------------------------------------------------------------------------------------------------------------*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"

uint32_t bpf_vm_printf(f12r_t *bpf, uint32_t fmt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
    (void)bpf;
    return printf((char*)(uintptr_t)fmt, a2, a3, a4, a5);
}
#pragma GCC diagnostic pop

uint32_t bpf_vm_store_local(f12r_t *bpf, uint32_t key, uint32_t value, uint32_t a3, uint32_t a4, uint32_t a5)
{
    (void)a3;
    (void)a4;
    (void)a5;
    return (uint32_t)bpf_store_update_local(bpf, key, value);
}

uint32_t bpf_vm_store_global(f12r_t *bpf, uint32_t key, uint32_t value, uint32_t a3, uint32_t a4, uint32_t a5)
{
    (void)bpf;
    (void)a3;
    (void)a4;
    (void)a5;
    return (uint32_t)bpf_store_update_global(key, value);
}

uint32_t bpf_vm_fetch_local(f12r_t *bpf, uint32_t key, uint32_t value, uint32_t a3, uint32_t a4, uint32_t a5)
{
    (void)bpf;
    (void)a3;
    (void)a4;
    (void)a5;
    if (f12r_store_allowed(bpf, (void*)value, sizeof(uint32_t)) < 0) {
        return -1;
    }
    return (uint32_t)bpf_store_fetch_local(bpf, key, (uint32_t*)(uintptr_t)value);
}

uint32_t bpf_vm_fetch_global(f12r_t *bpf, uint32_t key, uint32_t value, uint32_t a3, uint32_t a4, uint32_t a5)
{
    (void)bpf;
    (void)a3;
    (void)a4;
    (void)a5;
    if (f12r_store_allowed(bpf, (void*)value, sizeof(uint32_t)) < 0) {
        return -1;
    }
    return (uint32_t)bpf_store_fetch_global(key, (uint32_t*)(uintptr_t)value);
}



uint32_t bpf_vm_memcpy(f12r_t *bpf, uint32_t dest_p, uint32_t src_p, uint32_t size, uint32_t a4, uint32_t a5)
{
    (void)bpf;
    (void)a4;
    (void)a5;

    void *dest = (void *)(uintptr_t)dest_p;
    const void *src = (const void *)(uintptr_t)src_p;

    return (uintptr_t) memcpy(dest, src, size);
}

/*By Ahmad*/
uint32_t bpf_vm_malloc(f12r_t *bpf, uint32_t size, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
    (void)bpf;
    (void)a2;
    (void)a3;
    (void)a4;
    (void)a5;
    
    return (uintptr_t) malloc(size);
}


#ifdef MODULE_SAUL_REG
uint32_t f12r_vm_saul_reg_find_nth(f12r_t *f12r, uint64_t *regs)
{
    (void)f12r;
    int pos = (int)regs[0];
    saul_reg_t *reg = saul_reg_find_nth(pos);
    return (uint32_t)(intptr_t)reg;
}

uint32_t f12r_vm_saul_reg_find_type(f12r_t *f12r, uint64_t *regs)
{
    (void)f12r;

    saul_reg_t *reg = saul_reg_find_type(regs[0]);
    return (uint32_t)(intptr_t)reg;
}

uint32_t f12r_vm_saul_reg_read(f12r_t *f12r, uint64_t *regs)
{
    (void)f12r;

    saul_reg_t *dev = (saul_reg_t*)(intptr_t)regs[0];
    phydat_t *data = (phydat_t*)(intptr_t)regs[1];

    int res = saul_reg_read(dev, data);
    return (uint32_t)res;
}
#endif


#ifdef MODULE_ZTIMER
uint32_t f12r_vm_ztimer_now(f12r_t *f12r, uint64_t *regs)
{
    (void)f12r;
    (void)regs;
    return ztimer_now(ZTIMER_USEC);
}
uint32_t f12r_vm_ztimer_periodic_wakeup(f12r_t *f12r, uint64_t *regs)
{
    (void)f12r;

    uint32_t *last = (uint32_t*)(intptr_t)regs[0];

    ztimer_periodic_wakeup(ZTIMER_USEC, last, regs[1]);
    return 0;
}
#endif


f12r_call_t f12r_get_external_call(uint32_t num)
{
    switch(num) {

        /*Memory Access*/
        case BPF_FUNC_BPF_PRINTF:
            return &bpf_vm_printf;
        case BPF_FUNC_BPF_MEMCPY:
            return &bpf_vm_memcpy;

#ifdef MODULE_UDP_FC_HOOK
        /*-----------------------------*/
        /*By Ahmad*/
        case BPF_FUNC_BPF_MALLOC:
            return &bpf_vm_malloc;
        /*MALLOC PKTBUF*/
        case BPF_FUNC_BPF_GNRC_PKT_BUFF_START_WRITE_MALLOC:
            return &bpf_gnrc_pktbuf_start_write_malloc;
        case BPF_FUNC_BPF_GNRC_PKT_BUFF_RELEASE_MALLOC:
            return &bpf_gnrc_pktbuf_release_malloc;
        case BPF_FUNC_BPF_GNRC_PKT_BUFF_MARK_MALLOC:
            return &bpf_gnrc_pktbuf_mark_malloc;
        
        /*Static PKTBUF*/
        case BPF_FUNC_BPF_GNRC_PKT_BUFF_START_WRITE_STATIC:
            return &bpf_gnrc_pktbuf_start_write_static;
        case BPF_FUNC_BPF_GNRC_PKT_BUFF_RELEASE_STATIC:
            return &bpf_gnrc_pktbuf_release_static;
        case BPF_FUNC_BPF_GNRC_PKT_BUFF_MARK_STATIC:
            return &bpf_gnrc_pktbuf_mark_static;

        /*PKT*/
        case BPF_FUNC_BPF_GNRC_PKT_SEARCH_TYPE:
            return &bpf_gnrc_pktsnip_search_type;
        case BPF_FUNC_BPF_GNRC_PKT_SET_TYPE:
            return &bpf_gnrc_pktsnip_set_type;
        case BPF_FUNC_BPF_GNRC_PKT_GET_TYPE:
            return &bpf_gnrc_pktsnip_get_type; 
        
        case BPF_FUNC_BPF_GNRC_PKT_SET_NEXT:
            return &bpf_gnrc_pktsnip_set_next;
        case BPF_FUNC_BPF_GNRC_PKT_GET_NEXT:
            return &bpf_gnrc_pktsnip_get_next;

        case BPF_FUNC_BPF_GNRC_PKT_GET_SIZE:
            return &bpf_gnrc_pktsnip_get_size;

        case BPF_FUNC_BPF_GNRC_PKT_GET_DATA:
            return &bpf_gnrc_pktsnip_get_data;

        case BPF_FUNC_BPF_GNRC_PKT_GET_PKT_LEN:
            return &bpf_gnrc_get_pkt_len;
        
        /*UDP*/
        case BPF_FUNC_BPF_GNRC_PKT_GET_UDP_HCSUM:
            return &bpf_gnrc_pktsnip_get_udp_hcsum;
        case BPF_FUNC_BPF_GNRC_CALC_UDP_CSUM:
            return &bpf_gnrc_udp_calc_csum;
        case BPF_FUNC_BPF_GNRC_PKT_GET_UDP_DST_PORT:
            return &bpf_gnrc_pktsnip_get_udp_dst_port;
        case BPF_FUNC_BPF_GNRC_PKT_SET_UDP_LEN:
            return &bpf_gnrc_pktsnip_set_udp_len;

        /*NETAPI*/
        case BPF_FUNC_BPF_GNRC_NETAPI_DISPATCH_SEND:
            return &bpf_gnrc_netapi_dispatch_send;
        case BPF_FUNC_BPF_GNRC_NETAPI_DISPATCH_RX:
            return &bpf_gnrc_netapi_dispatch_receive;

        /*CROSS*/
        case BPF_FUNC_BPF_CALC_CSUM:
            return &bpf_calc_csum;

        /*-----------------------------*/
#endif
        
#ifdef MODULE_SAUL_REG
        case BPF_FUNC_BPF_SAUL_REG_FIND_NTH:
            return &f12r_vm_saul_reg_find_nth;
        case BPF_FUNC_BPF_SAUL_REG_FIND_TYPE:
            return &f12r_vm_saul_reg_find_type;
        case BPF_FUNC_BPF_SAUL_REG_READ:
            return &f12r_vm_saul_reg_read;
#endif

#ifdef MODULE_GCOAP
        case BPF_FUNC_BPF_GCOAP_RESP_INIT:
            return &f12r_vm_gcoap_resp_init;
        case BPF_FUNC_BPF_COAP_OPT_FINISH:
            return &f12r_vm_coap_opt_finish;
        case BPF_FUNC_BPF_COAP_ADD_FORMAT:
            return &f12r_vm_coap_add_format;
        case BPF_FUNC_BPF_COAP_GET_PDU:
            return &f12r_vm_coap_get_pdu;
#endif

        default:
            return NULL;
    }
}

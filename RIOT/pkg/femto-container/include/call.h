/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BPF_CALL_H
#define BPF_CALL_H

#include <stdint.h>
#include "femtocontainer/femtocontainer.h"

#ifdef __cplusplus
extern "C" {
#endif



uint32_t f12r_vm_memcpy(f12r_t *f12r, uint32_t dest_p, uint32_t src_p, uint32_t size, uint32_t a4, uint32_t a5);

uint32_t bpf_vm_printf(f12r_t *bpf, uint32_t fmt, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t bpf_vm_store_local(f12r_t *bpf, uint32_t fmt, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t bpf_vm_store_global(f12r_t *bpf, uint32_t fmt, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t bpf_vm_fetch_local(f12r_t *bpf, uint32_t fmt, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t bpf_vm_fetch_global(f12r_t *bpf, uint32_t fmt, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t bpf_vm_memcpy(f12r_t *bpf, uint32_t dest_p, uint32_t src_p, uint32_t size, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_now_ms(f12r_t *bpf, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_saul_reg_find_nth(f12r_t *bpf, uint32_t nth, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_saul_reg_find_type(f12r_t *bpf, uint32_t type, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_saul_reg_read(f12r_t *bpf, uint32_t dev_p, uint32_t data_p, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_gcoap_resp_init(f12r_t *bpf, uint32_t coap_ctx_p, uint32_t resp_code_u, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_coap_opt_finish(f12r_t *bpf, uint32_t coap_ctx_p, uint32_t flags_u, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_fmt_s16_dfp(f12r_t *bpf, uint32_t out_p, uint32_t val, uint32_t fp_digits, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_fmt_u32_dec(f12r_t *bpf, uint32_t out_p, uint32_t val, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_coap_add_format(f12r_t *bpf, uint32_t coap_ctx_p, uint32_t format, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_coap_get_pdu(f12r_t *bpf, uint32_t coap_ctx_p, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);


uint32_t bpf_vm_ztimer_now(f12r_t *bpf, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_ztimer_periodic_wakeup(f12r_t *bpf, uint32_t last_wakeup_p, uint32_t period, uint32_t a3, uint32_t a4, uint32_t a5);


/***AHMAD***/
/*Memory*/
uint32_t bpf_vm_malloc(f12r_t *bpf, uint32_t size, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

/*GNRC*/
//pktbuf_static.c
uint32_t bpf_gnrc_pktbuf_start_write_static(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_gnrc_pktbuf_release_static(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_gnrc_pktbuf_mark_static(f12r_t *bpf, uint32_t pkt, uint32_t size, uint32_t type, uint32_t a4, uint32_t a5);

//gnrc_pkt.c
uint32_t bpf_gnrc_pktsnip_search_type(f12r_t *bpf, uint32_t pkt, uint32_t type, uint32_t a3, uint32_t a4, uint32_t a5);

uint32_t bpf_gnrc_pktsnip_get_type(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_gnrc_pktsnip_set_type(f12r_t *bpf, uint32_t pkt, uint32_t type, uint32_t a3, uint32_t a4, uint32_t a5);

uint32_t bpf_gnrc_pktsnip_get_next(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_gnrc_pktsnip_set_next(f12r_t *bpf, uint32_t pkt, uint32_t next, uint32_t a3, uint32_t a4, uint32_t a5);

uint32_t bpf_gnrc_pktsnip_get_data(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_gnrc_pktsnip_get_size(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

uint32_t bpf_gnrc_pktsnip_get_udp_hcsum(f12r_t *bpf, uint32_t udp_hdr, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_gnrc_pktsnip_get_udp_dst_port(f12r_t *bpf, uint32_t udp_hdr, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_gnrc_pktsnip_set_udp_len(f12r_t *bpf, uint32_t udp_hdr, uint32_t size, uint32_t a3, uint32_t a4, uint32_t a5);

uint32_t bpf_gnrc_get_pkt_len(f12r_t *bpf, uint32_t pkt, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);

//cross layer
uint32_t bpf_gnrc_udp_calc_csum(f12r_t *bpf, uint32_t hdr, uint32_t pseudo_hdr, uint32_t a3, uint32_t a4, uint32_t a5);

uint32_t bpf_gnrc_netapi_dispatch_send(f12r_t *bpf, uint32_t type, uint32_t demux_ctx, uint32_t pkt, uint32_t a4, uint32_t a5);
uint32_t bpf_gnrc_netapi_dispatch_receive(f12r_t *bpf, uint32_t type, uint32_t demux_ctx, uint32_t pkt, uint32_t a4, uint32_t a5);

uint32_t bpf_calc_csum(f12r_t *bpf, uint32_t hdr, uint32_t pseudo_hdr, uint32_t payload, uint32_t a4, uint32_t a5);
/***AHMAD***/

uint32_t bpf_vm_ztimer_now(f12r_t *bpf, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
uint32_t bpf_vm_ztimer_periodic_wakeup(f12r_t *bpf, uint32_t last_wakeup_p, uint32_t period, uint32_t a3, uint32_t a4, uint32_t a5);
#ifdef __cplusplus
}
#endif
#endif /* BPF_CALL_H */


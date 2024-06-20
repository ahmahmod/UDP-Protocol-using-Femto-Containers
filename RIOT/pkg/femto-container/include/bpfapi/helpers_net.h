#ifndef BPF_BPFAPI_HELPERS_NET_H
#define BPF_BPFAPI_HELPERS_NET_H

#include <stdint.h>
#include "bpf/shared.h"
#include "phydat.h"


#ifdef __cplusplus
extern "C" {
#endif

/*PKTBUF*/

static void *(*bpf_gnrc_pktbuf_start_write_static)(void* pkt) = (void *) BPF_FUNC_BPF_GNRC_PKT_BUFF_START_WRITE_STATIC;
static void *(*bpf_gnrc_pktbuf_release_static)(void* pkt) = (void *) BPF_FUNC_BPF_GNRC_PKT_BUFF_RELEASE_STATIC;
static void *(*bpf_gnrc_pktbuf_mark_static)(void* pkt, uint32_t size, uint32_t type) = (void *) BPF_FUNC_BPF_GNRC_PKT_BUFF_MARK_STATIC;

/*PKT*/
static void *(*bpf_gnrc_pktsnip_search_type)(void* pkt, uint32_t type) = (void *) BPF_FUNC_BPF_GNRC_PKT_SEARCH_TYPE;
static void *(*bpf_gnrc_pktsnip_set_type)(void* pkt, uint32_t type) = (void *) BPF_FUNC_BPF_GNRC_PKT_SET_TYPE;
static void *(*bpf_gnrc_pktsnip_get_type)(void* pkt) = (void *) BPF_FUNC_BPF_GNRC_PKT_GET_TYPE;
static void *(*bpf_gnrc_pktsnip_set_next)(void* pkt, void* next) = (void *) BPF_FUNC_BPF_GNRC_PKT_SET_NEXT;
static void *(*bpf_gnrc_pktsnip_get_next)(void* pkt) = (void *) BPF_FUNC_BPF_GNRC_PKT_GET_NEXT;
static void *(*bpf_gnrc_pktsnip_get_size)(void* pkt) = (void *) BPF_FUNC_BPF_GNRC_PKT_GET_SIZE;
static void *(*bpf_gnrc_pktsnip_get_data)(void* pkt) = (void *) BPF_FUNC_BPF_GNRC_PKT_GET_DATA;
static void *(*bpf_gnrc_get_pkt_len)(void* pkt) = (void *) BPF_FUNC_BPF_GNRC_PKT_GET_PKT_LEN;

/*UDP*/
static void *(*bpf_gnrc_pktsnip_get_udp_hcsum)(void* udp_hdr) = (void *) BPF_FUNC_BPF_GNRC_PKT_GET_UDP_HCSUM;
static void *(*bpf_gnrc_udp_calc_csum)(void* hdr, void* pseudo_hdr) = (void *) BPF_FUNC_BPF_GNRC_CALC_UDP_CSUM;
static void *(*bpf_gnrc_pktsnip_get_udp_dst_port)(void* udp_hdr) = (void *) BPF_FUNC_BPF_GNRC_PKT_GET_UDP_DST_PORT;
static void *(*bpf_gnrc_pktsnip_set_udp_len)(void* udp_hdr, uint32_t size) = (void *) BPF_FUNC_BPF_GNRC_PKT_SET_UDP_LEN;


/*NETAPI*/
static void *(*bpf_gnrc_netapi_dispatch_send)(uint32_t type, uint32_t demux_ctx, void* pkt) = (void *) BPF_FUNC_BPF_GNRC_NETAPI_DISPATCH_SEND;
static void *(*bpf_gnrc_netapi_dispatch_receive)(uint32_t type, uint32_t demux_ctx, void* pkt) = (void *) BPF_FUNC_BPF_GNRC_NETAPI_DISPATCH_RX;

/*CROSS*/
static void *(*bpf_calc_csum)(void* hdr, void* pseudo_hdr, void* payload) = (void *) BPF_FUNC_BPF_CALC_CSUM;

#ifdef __cplusplus

}
#endif
#endif /* BPF_APPLICATION_CALL_H */

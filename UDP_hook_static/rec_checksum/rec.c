#include <stdint.h>

#include "bpfapi/helpers.h"
#include "bpfapi/helpers_net.h"
#include "net/gnrc.h"
#include "net/gnrc/udp.h"

/* return values*/
enum{
    ERROR = -1,
    OK,
};

/**
 * @brief   Receive UDP packet from network layer and forward to the proper port.
 * @pre @p  pkt != NULL
 *
 * @param[in] pkt       A gnrc_pktsnip_t pointer to the pkt sent by network layer.
 *                      It represents the context of the FC.
 *                      
 *
 * @return  OK, in case of successful handling
 * @return  ERROR, in case of any error which stop the processing
 */

uint32_t rec(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *udp = NULL;
    gnrc_pktsnip_t *ipv6 = NULL;
    gnrc_pktsnip_t *next = NULL;
    udp_hdr_t *hdr = NULL;
    uint32_t port, type, size;
    bool cond;

    
    /* mark UDP header */
    if (pkt == NULL){
        bpf_gnrc_pktbuf_release_static(pkt);
        return ERROR;
    }
    
    /* mark UDP header */
    udp = bpf_gnrc_pktbuf_start_write_static(pkt);
    if (udp == NULL) {
        bpf_gnrc_pktbuf_release_static(pkt);
        return ERROR;
    }

    /* start use of the protected packet*/
    pkt = udp;
    /* find IPV6 snip in the packet*/
    ipv6 = bpf_gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6); 
    
    /* obtain data for marking */
    type = (int)(uintptr_t)bpf_gnrc_pktsnip_get_type(pkt->next);
    size = (uint32_t)(uintptr_t)bpf_gnrc_pktsnip_get_size(pkt->next);
    next = (gnrc_pktsnip_t*)(uintptr_t)bpf_gnrc_pktsnip_get_next(pkt);

    if((next != NULL) && (type == GNRC_NETTYPE_UDP) && (size == sizeof(udp_hdr_t))){
         /* UDP header was already marked. Take it. */
        udp = next;
    }
    
    else{
        udp = bpf_gnrc_pktbuf_mark_static(pkt, sizeof(udp_hdr_t), GNRC_NETTYPE_UDP);
        if (udp == NULL) {
            bpf_gnrc_pktbuf_release_static(pkt);
            return ERROR;
        }
    }

    /* mark payload as Type: UNDEF */
    bpf_gnrc_pktsnip_set_type(pkt, GNRC_NETTYPE_UNDEF);

    /* get explicit pointer to UDP header */
    hdr = (udp_hdr_t *)(uintptr_t)bpf_gnrc_pktsnip_get_data(udp);
    
    /* check the checksum*/
    if (hdr!=NULL){
        if ((bpf_gnrc_pktsnip_get_udp_hcsum(hdr) == 0) || ((uint16_t)(uintptr_t) bpf_calc_csum(udp, ipv6, pkt) != 0xFFFF)) {
            bpf_gnrc_pktbuf_release_static(pkt);
            return ERROR;
        }
    }

    /* send the packet to the proper port*/
    if (hdr!=NULL && pkt!=NULL){
        port = (uint32_t)(uintptr_t)bpf_gnrc_pktsnip_get_udp_dst_port(hdr);
        int send_port = (int)(uintptr_t)bpf_gnrc_netapi_dispatch_receive(GNRC_NETTYPE_UDP, port, pkt);

        if (!send_port) {
            bpf_gnrc_pktbuf_release_static(pkt);
            return ERROR;
        }
    }

    return OK;
}
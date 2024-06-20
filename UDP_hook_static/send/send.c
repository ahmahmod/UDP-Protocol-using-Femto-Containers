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
 * @brief   Send UDP packet from application layer to the network layer.
 * @pre @p  pkt != NULL
 *
 * @param[in] pkt       A gnrc_pktsnip_t pointer to the pkt sent by the application.
 *                      It represents the context of the FC.
 *                      
 *
 * @return  OK, in case of successful handling
 * @return  ERROR, in case of any error which stop the processing
 */

uint32_t send(gnrc_pktsnip_t *pkt)
{
    udp_hdr_t *hdr;
    gnrc_pktsnip_t *udp_snip, *tmp;
    gnrc_nettype_t udp_snip_type;
    gnrc_nettype_t target_type = (gnrc_nettype_t)(uintptr_t) bpf_gnrc_pktsnip_get_type(pkt);

    /* write protect first header */
    if (pkt != NULL){
        tmp = bpf_gnrc_pktbuf_start_write_static(pkt);
        if (tmp == NULL) {
            bpf_gnrc_pktbuf_release_static(pkt);
            return ERROR;
        }
    }

    /* start use of the protected packet*/
    pkt = tmp;

    /* handle the UDP snip*/
    udp_snip = (gnrc_pktsnip_t*)(uintptr_t) bpf_gnrc_pktsnip_get_next(tmp);
    udp_snip_type = (gnrc_nettype_t)(uintptr_t)bpf_gnrc_pktsnip_get_type(udp_snip);

    /* get and write protect until udp snip */
    while ((udp_snip != NULL) && (udp_snip_type != GNRC_NETTYPE_UDP)) {
        udp_snip = bpf_gnrc_pktbuf_start_write_static(udp_snip);
        if (udp_snip == NULL) {
            bpf_gnrc_pktbuf_release_static(pkt);
            return ERROR;
        }
        bpf_gnrc_pktsnip_set_next(tmp, udp_snip);
        tmp = udp_snip;
        udp_snip = bpf_gnrc_pktsnip_get_next(udp_snip);
        udp_snip_type = (int)(uintptr_t)bpf_gnrc_pktsnip_get_type(udp_snip);
    }

    /* write protect UDP snip */
    udp_snip = bpf_gnrc_pktbuf_start_write_static(udp_snip);
    if (udp_snip == NULL) {
        bpf_gnrc_pktbuf_release_static(pkt);
        return ERROR;
    }

    /* extract UDP header for manipulation*/
    bpf_gnrc_pktsnip_set_next(tmp, udp_snip);
    hdr = (udp_hdr_t *)bpf_gnrc_pktsnip_get_data(udp_snip);
    
    /* fill in size field */
    uint32_t len = (uint32_t)(uintptr_t) bpf_gnrc_get_pkt_len(udp_snip);
    bpf_gnrc_pktsnip_set_udp_len(hdr, len);

    /* if the first snip is NETIF snip, make the target IPV6 */
    if (target_type == GNRC_NETTYPE_NETIF) {
        target_type = GNRC_NETTYPE_IPV6;
    }

    /* forward packet to the network layer */
    if (pkt!=NULL){
        if (!bpf_gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
            bpf_gnrc_pktbuf_release_static(pkt);
            return ERROR;
        }
    }
    
    return OK;

}

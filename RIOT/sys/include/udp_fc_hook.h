/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sched_rbpf rbpf scheduler hook
 * @ingroup     sys
 *
 * @{
 *
 * @file
 * @brief       Scheduler rbpf callback
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef UDP_FC_HOOK
#define UDP_FC_HOOK

#include <stdint.h>
#include "net/gnrc.h"
#include "femtocontainer/femtocontainer.h"

#ifdef __cplusplus
 extern "C" {
#endif


/**
 *  @brief  Execute the UDP packet sender application
 */
void udp_fc_send(gnrc_pktsnip_t* ctx);

/**
 *  @brief  Execute the UDP packet receiver application
 */
void udp_fc_recv(gnrc_pktsnip_t* ctx);

/*Add UDP hooks*/
/**
 *  @brief  Register the UDP send hook's application
 */
void udp_fc_send_add_hook(f12r_hook_t *hook);

/**
 *  @brief  Register the UDP receive hook's application
 */
void udp_fc_recv_add_hook(f12r_hook_t *hook);

#ifdef __cplusplus
}
#endif

#endif /* SCHED_RBPF_H */
/** @} */


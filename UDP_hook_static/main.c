/*
 * Copyright (c) 2024 Ahmad Mahmod <https://ahmahmod.github.io/>

 */

/**
 * @ingroup     UDP
 * @{
 *
 * @file
 * @brief       UDP using FCs
 *
 * @author     Ahmad Mahmod <https://ahmahmod.github.io/>
 *
 * @}
 */
#include <stdio.h>
#include "shell.h"
#include "msg.h"

#include "blob/rec/rec.bin.h"
#include "blob/send/send.bin.h"

#ifdef MODULE_UDP_FC_HOOK
#include "udp_fc_hook.h"
#include "femtocontainer/femtocontainer.h"
#endif

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#ifdef MODULE_UDP_FC_HOOK
    /* Define the applications */
    f12r_t bpf_send = {
        .application = send_bin,                  /* The send.bin content */
        .application_len = sizeof(send_bin),      /* Length of the application */
    };

    f12r_t bpf_recv = {
        .application = rec_bin,                  /* The rec.bin content */
        .application_len = sizeof(rec_bin),      /* Length of the application */
    };

    /* Define the hooks */
    static f12r_hook_t send_hook = {
        .application = &bpf_send,
    };

    static f12r_hook_t recv_hook = {
        .application = &bpf_recv,
    };
#endif


/*Use the application which call the udp send FC*/
extern int udp_cmd(int argc, char **argv);

/* Shell commands defination.
This function is called when the user types the command name into the shell.
1.A string representing the command name (e.g., "udp").
2.A string describing what the command does (e.g., "send data over UDP and listen on UDP ports").
3.A function pointer that points to the implementation of the command (e.g., udp_cmd).
*/
static const shell_command_t shell_commands[] = {
  { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
  { NULL, NULL, NULL }
};

int main(void)
{
    /* for the thread running the shell */
    //msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    
    puts("UDP bpf protocol");
    //f12r_init();
    #ifdef MODULE_UDP_FC_HOOK
        udp_fc_send_add_hook(&send_hook);
        udp_fc_recv_add_hook(&recv_hook);
        check_hook();
    #endif

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    
    return 0;
}

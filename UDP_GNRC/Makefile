# name of the example application
APPLICATION = bpf_tutorial

# Default is to build for the "native" target. Allows overriding
BOARD ?= iotlab-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE = ../RIOT

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error

# Specify the mandatory networking module for a IPv6 routing node
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp
# Add a routing protocol
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
# Dump
USEMODULE += gnrc_pktdump

# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo
#USEMODULE += gnrc_udp_cmd

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl


# Include the bpf module
USEMODULE += gnrc
USEMODULE += gnrc_pktbuf_static

USEPKG += femto-container
#USEMODULE += udp_fc_hook
USEMODULE += ztimer


#TEST_ON_CI_WHITELIST += native

# Set GNRC_PKTBUF_SIZE via CFLAGS if not being set via Kconfig.
ifndef CONFIG_GNRC_PKTBUF_SIZE
  CFLAGS += -DCONFIG_GNRC_PKTBUF_SIZE=400
endif

# Instead of simulating an Ethernet connection, we can also simulate
# an IEEE 802.15.4 radio using ZEP

USE_ZEP ?= 0

# set the ZEP port for native
ZEP_PORT_BASE ?= 17754
ifeq (1,$(USE_ZEP))
  TERMFLAGS += -z [::1]:$(ZEP_PORT_BASE)
  USEMODULE += socket_zep
endif


# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1
DEFAULT_CHANNEL=15
# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
include $(RIOTMAKE)/default-radio-settings.inc.mk

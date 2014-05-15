/*
 * cc3000_netapp.c
 *
 *  Created on: 12.09.2013
 *      Author: Johannes
 */

#include "cc3000_netapp.h"
#include "cc3000_platform.h"
#include "cc3000_event_handler.h"
#include "cc3000_general.h"
#include "cc3000_hci.h"

#define MIN_TIMER_VAL_SECONDS      20
#define MIN_TIMER_SET(t)    if ((0 != t) && (t < MIN_TIMER_VAL_SECONDS)) \
                            { \
                                t = MIN_TIMER_VAL_SECONDS; \
                            }

//*****************************************************************************
//
//!  netapp_dhcp
//!
//!  @param  aucIP
//!  @param  aucSubnetMask
//!  @param  aucDefaultGateway
//!  @param  aucDNSServer
//!
//!  @return      none, but cc3000_get_return_uint32() should read 0 on sucess
//!
//!  @brief       netapp_dhcp is used to configure the network interface,
//!               static or dynamic (DHCP).\n In order to activate DHCP mode,
//!               aucIP, aucSubnetMask, aucDefaultGateway must be 0.
//!               The default mode of CC3000 is DHCP mode.
//!               Note that the configuration is saved in non volatile memory
//!               and thus preserved over resets.
//!
//! @note         If the mode is altered a reset of CC3000 device is required
//!               in order to apply changes.Also note that asynchronous event
//!               of DHCP_EVENT, which is generated when an IP address is
//!               allocated either by the DHCP server or due to static
//!               allocation is generated only upon a connection to the
//!               AP was established.
//!
//*****************************************************************************
void cc3000_netapp_dhcp(uint32 aucIP, uint32 aucSubnetMask, uint32 aucDefaultGateway, uint32 aucDNSServer){

	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_NETAPP_DHCP, 20);
	cc3000_hci_send_uint32(aucIP);
	cc3000_hci_send_uint32(aucSubnetMask);
	cc3000_hci_send_uint32(aucDefaultGateway);
	cc3000_hci_send_uint32(0);
	cc3000_hci_send_uint32(aucDNSServer);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_NETAPP_DHCP);

}

//*****************************************************************************
//
//!  netapp_timeout_values
//!
//!  @param  aucDHCP    DHCP lease time request, also impact
//!                     the DHCP renew timeout. Range: [0-0xffffffff] seconds,
//!                     0 or 0xffffffff == infinity lease timeout.
//!                     Resolution:10 seconds. Influence: only after
//!                     reconnecting to the AP.
//!                     Minimal bound value: MIN_TIMER_VAL_SECONDS - 20 seconds.
//!                     The parameter is saved into the CC3000 NVMEM.
//!                     The default value on CC3000 is 14400 seconds.
//!
//!  @param  aucARP     ARP refresh timeout, if ARP entry is not updated by
//!                     incoming packet, the ARP entry will be  deleted by
//!                     the end of the timeout.
//!                     Range: [0-0xffffffff] seconds, 0 == infinity ARP timeout
//!                     Resolution: 10 seconds. Influence: on runtime.
//!                     Minimal bound value: MIN_TIMER_VAL_SECONDS - 20 seconds
//!                     The parameter is saved into the CC3000 NVMEM.
//!	                    The default value on CC3000 is 3600 seconds.
//!
//!  @param  aucKeepalive   Keepalive event sent by the end of keepalive timeout
//!                         Range: [0-0xffffffff] seconds, 0 == infinity timeout
//!                         Resolution: 10 seconds.
//!                         Influence: on runtime.
//!                         Minimal bound value: MIN_TIMER_VAL_SECONDS - 20 sec
//!                         The parameter is saved into the CC3000 NVMEM.
//!                         The default value on CC3000 is 10 seconds.
//!
//!  @param  aucInactivity   Socket inactivity timeout, socket timeout is
//!                          refreshed by incoming or outgoing packet, by the
//!                          end of the socket timeout the socket will be closed
//!                          Range: [0-0xffffffff] sec, 0 == infinity timeout.
//!                          Resolution: 10 seconds. Influence: on runtime.
//!                          Minimal bound value: MIN_TIMER_VAL_SECONDS - 20 sec
//!                          The parameter is saved into the CC3000 NVMEM.
//!	                         The default value on CC3000 is 60 seconds.
//!
//!  @return       none, but cc3000_get_return_uint32() should read 0 on sucess
//!
//!  @brief       Set new timeout values. Function set new timeout values for:
//!               DHCP lease timeout, ARP  refresh timeout, keepalive event
//!               timeout and socket inactivity timeout
//!
//! @note         If a parameter set to non zero value which is less than 20s,
//!               it will be set automatically to 20s.
//!
//*****************************************************************************
void netapp_timeout_values(uint32 aucDHCP, uint32 aucARP, uint32 aucKeepalive, uint32 aucInactivity){


	// Set minimal values of timers
	MIN_TIMER_SET(aucDHCP)
    MIN_TIMER_SET(aucARP)
	MIN_TIMER_SET(aucKeepalive)
	MIN_TIMER_SET(aucInactivity)

	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_NETAPP_SET_TIMERS, 16);
	cc3000_hci_send_uint32(aucDHCP);
	cc3000_hci_send_uint32(aucARP);
	cc3000_hci_send_uint32(aucKeepalive);
	cc3000_hci_send_uint32(aucInactivity);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_NETAPP_SET_TIMERS);
}

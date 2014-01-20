/*
 * cc3000_flow_handler.c
 *
 *  Created on: 06.09.2013
 *      Author: Johannes
 */

#include "cc3000_flow_handler.h"
#include "cc3000_event_handler.h"
#include "cc3000_general.h"
#include "cc3000_socket.h"
#include "cc3000_netapp.h"
#include "cc3000_wlan.h"
#include "cc3000_spi.h"
#include "cc3000_hci.h"

#include <string.h>

ap_entry_t result;
ip_addr_t tcp_ip;

int8 socket;

fd_set readsock, writesock, exceptsock;
sockaddr  tSocketAddr;
sockaddr clientaddr;
socklen_t addrlen;

uint8 clientIP[4];
int16 clientstatus;

uint16 counter = 0;

int16  nonBlocking = 0;
int32 maxFD;
timeval timeout;

// buffer for TCP receive data
uint8 recv_buffer[64];
// number of bytes received
uint8 recv_bytes;
// buffer for TCP send
uint8 tcp_tx_buff[64] = "Hello World!";

// AP data
uint8 ssid[10] = "conn_test";
uint8 key[16] = "12345678";

// port for TCP client and/or server
uint16 port = 3333;

// IP Adress of server (in case we are a client and want to connect to it)
#define SERVER_IP_0		192
#define SERVER_IP_1		168
#define SERVER_IP_2		115
#define SERVER_IP_3		11

// own IP Adress (for netapp)
#define OWN_IP_0		192
#define OWN_IP_1		168
#define OWN_IP_2		115
#define OWN_IP_3		161
uint32 cc3000_aucIP	= (OWN_IP_3<<24)+(OWN_IP_2<<16)+(OWN_IP_1<<8)+(OWN_IP_0);

// gateway IP Adress (for netapp)
#define GW_IP_0			192
#define GW_IP_1			168
#define GW_IP_2			115
#define GW_IP_3			1
uint32	cc3000_aucGateway = (GW_IP_3<<24)+(GW_IP_2<<16)+(GW_IP_1<<8)+(GW_IP_0);

// dns IP Adress (for netapp)
#define DNS_IP_0		192
#define DNS_IP_1		168
#define DNS_IP_2		115
#define DNS_IP_3		1
uint32	cc3000_aucDNSServer = (DNS_IP_3<<24)+(DNS_IP_2<<16)+(DNS_IP_1<<8)+(DNS_IP_0);

// subnet (for netapp)
#define SUBN_0			255
#define SUBN_1			255
#define SUBN_2			255
#define SUBN_3			0
uint32	cc3000_subNet = (SUBN_3<<24)+(SUBN_2<<16)+(SUBN_1<<8)+(SUBN_0);

uint32 cc3000_active_state = 0;
uint32 cc3000_state_sent = 0;

//*****************************************************************************
//
//! cc3000_fh_set_active
//!
//!  @param  	state	next state to be executed
//!
//!  @return  	none
//!
//!  @brief  	sets the next state to be executed
//!
//
//*****************************************************************************
void cc3000_fh_set_active(uint8 state){
	cc3000_active_state = state;
}
//*****************************************************************************
//
//! cc3000_fh_set_active_sent
//!
//!  @param  	state	the state that has been sent to the cc3000
//!
//!  @return  	none
//!
//!  @brief  	sets a flag to determine if a desired command has been sent
//!
//
//*****************************************************************************
void _cc3000_fh_set_active_sent(uint8 state){
	cc3000_state_sent = state;
}
//*****************************************************************************
//
//! cc3000_fh_get_active_sent
//!
//!  @param  	state	the state to check
//!
//!  @return  	returns 1 if the state has been sent
//!				returns 0 if the state has not been sent
//!
//!  @brief  	checks if the desired state has been sent
//!
//
//*****************************************************************************
uint8 _cc3000_fh_get_active_sent(uint8 state){
	return (state == cc3000_state_sent) ? 1 : 0;
}
//*****************************************************************************
//
//! cc3000_flow_handler
//!
//!  @param  	none
//!
//!  @return  	none
//!
//!  @brief  	this is the main handler for the cc3000;
//!				it must be called periodically from the main loop;
//!				it can set new states automatically when the previous has been
//!				executed, or it can set the state to idle if nothing is to do
//!
//
//*****************************************************************************
void cc3000_flow_handler(void){

	int32 status = 0;

	// Note: if you want to send the same command again you will have to set
	// _cc3000_fh_set_active_sent(STATE_IDLE);
	// current flow: STATE_BUFFER_SIZE -> STATE_EVENT_MASK -> STATE_CON_POLICY -> STATE_WL_CONNECT
	// if you set the state to STATE_SC_OPEN then it will automaticly call:
	// STATE_SC_BIND -> STATE_SC_LISTEN -> STATE_SC_SOCOPT -> STATE_SC_ACCEPT and a tcp server is running
	// and will send "Ping" with a counter vaule a thousand times...

	switch(cc3000_active_state){

		// get buffer size
		case STATE_BUFFER_SIZE:
			// has the command been sent yet?
			if(!_cc3000_fh_get_active_sent(STATE_BUFFER_SIZE)){
				// if not, then send it
				cc3000_general_req_buffer_size();
				// set the flag to indicate that the command has been sent
				_cc3000_fh_set_active_sent(STATE_BUFFER_SIZE);
			}
			// has the cc3000 returned the expected opcode?
			else if(cc3000_is_ready()){
				// execute what is necessary
				cc3000_general_read_buffer_size(&cc3000_free_buffers,&cc3000_buffer_length);
				// set the next state
				cc3000_fh_set_active(STATE_EVENT_MASK);
			}
			break;

		// set event mask
		case STATE_EVENT_MASK:
			if(!_cc3000_fh_get_active_sent(STATE_EVENT_MASK)){
				cc3000_wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE);
				_cc3000_fh_set_active_sent(STATE_EVENT_MASK);
			}
			else if(cc3000_is_ready()){
				status = cc3000_get_return_uint8(0);
				cc3000_fh_set_active(STATE_CON_POLICY);
				//cc3000_fh_set_active(STATE_IDLE);
			}
			break;

		// set connection policy
		case STATE_CON_POLICY:
			if(!_cc3000_fh_get_active_sent(STATE_CON_POLICY)){
				cc3000_wlan_set_connection_policy(fast_connect_disabled, open_ap_auto_connect_disabled, use_profiles_disabled);
				_cc3000_fh_set_active_sent(STATE_CON_POLICY);
			}
			else if(cc3000_is_ready()){
				printf("set con pol: %u\n\r",cc3000_get_return_uint8(0));
				cc3000_fh_set_active(STATE_WL_CONNECT);
			}
			break;

		// connect
		case STATE_WL_CONNECT:
			if(!_cc3000_fh_get_active_sent(STATE_WL_CONNECT)){
				cc3000_wlan_connect(ssid, WLAN_SEC_WPA2, key);
				_cc3000_fh_set_active_sent(STATE_WL_CONNECT);
			}
			else if(cc3000_is_ready()){
				status = cc3000_get_return_uint8(0);
				cc3000_fh_set_active(STATE_IDLE);
			}
			break;


		// scan parameter
		case STATE_WL_SCAN_PARA:
			if(!_cc3000_fh_get_active_sent(STATE_WL_SCAN_PARA)){
				cc3000_wlan_set_scan_params(60, 0x1fff);
				_cc3000_fh_set_active_sent(STATE_WL_SCAN_PARA);
			}
			else if(cc3000_is_ready()){
				status = cc3000_get_return_uint32(0);
				cc3000_fh_set_active(STATE_WL_SCAN);
			}
			break;


		// scan results
		case STATE_WL_SCAN:
			if(!_cc3000_fh_get_active_sent(STATE_WL_SCAN)){
				cc3000_wlan_req_scan_result();
				_cc3000_fh_set_active_sent(STATE_WL_SCAN);
			}
			else if(cc3000_is_ready()){
				cc3000_wlan_get_scan_result(&result);

				debug_str("Entry count: \n");
				debug_int(result.entry_count);
				debug_nl();
				debug_str("Scan Status = \n");
				switch (result.scan_status) {
					case 0: debug_str("Aged results\n"); break;
					case 1: debug_str("Results valid\n"); break;
					case 2: debug_str("No result\n"); break;
				}
				debug_str("This entry valid: \n");
				if (result.entry_valid) {
					debug_str("Yes\n");
				} else {
					debug_str("No\n");
				}
				debug_str("RSSI: \n");
				debug_int(result.rssi);
				debug_nl();
				debug_str("Security: \n");
				switch (result.security) {
					case 0: debug_str("Open\n"); break;
					case 1: debug_str("WEP\n"); break;
					case 2: debug_str("WPA\n"); break;
					case 3: debug_str("WPA2\n"); break;
				}
				debug_nl();
				debug_str("SSID: \n");
				debug_str(result.ssid);
				debug_nl();

				cc3000_fh_set_active(STATE_IDLE);
			}
			break;

			// open socket
			case STATE_SC_OPEN:
				if(!_cc3000_fh_get_active_sent(STATE_SC_OPEN)){
					cc3000_req_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
					_cc3000_fh_set_active_sent(STATE_SC_OPEN);
				}
				else if(cc3000_is_ready()){
					socket = (int8)cc3000_get_return_int32(0);
					set_socket_active_status(socket, SOCKET_STATUS_ACTIVE);
					printf("SOCKET: %i\n\r",socket);
					cc3000_fh_set_active(STATE_SC_BIND);
				}
				break;

			// connect to socket
			case STATE_SC_CONNECT:
				if(!_cc3000_fh_get_active_sent(STATE_SC_CONNECT)){
					printf("let's try to connect to the socket\n\r");
					tcp_ip.ip[0] = SERVER_IP_3;
					tcp_ip.ip[1] = SERVER_IP_2;
					tcp_ip.ip[2] = SERVER_IP_1;
					tcp_ip.ip[3] = SERVER_IP_0;
					cc3000_connect(socket, port, tcp_ip);
					_cc3000_fh_set_active_sent(STATE_SC_CONNECT);
				}
				else if(cc3000_is_ready()){
					printf("we are connected to the socket...\n");
					cc3000_fh_set_active(STATE_IDLE);
				}
				break;

			// bind to socket
			case STATE_SC_BIND:
				if(!_cc3000_fh_get_active_sent(STATE_SC_BIND)){
					 cc3000_bind(socket,port);
					_cc3000_fh_set_active_sent(STATE_SC_BIND);
				}
				else if(cc3000_is_ready()){
					printf("Bind: %u\n\r",cc3000_get_return_int8(0));
					cc3000_fh_set_active(STATE_SC_LISTEN);
				}
				break;

			// listen to socket
			case STATE_SC_LISTEN:
				if(!_cc3000_fh_get_active_sent(STATE_SC_LISTEN)){
					 cc3000_listen(socket);
					_cc3000_fh_set_active_sent(STATE_SC_LISTEN);
				}
				else if(cc3000_is_ready()){
					printf("Listen: %u\n\r",cc3000_get_return_int8(0));
					cc3000_fh_set_active(STATE_SC_SOCOPT);
				}
				break;

			// set socket options
			case STATE_SC_SOCOPT:
				if(!_cc3000_fh_get_active_sent(STATE_SC_SOCOPT)){
					cc3000_setsockopt(socket, SOL_SOCKET, SOCKOPT_ACCEPT_NONBLOCK, &nonBlocking, sizeof(nonBlocking));
					_cc3000_fh_set_active_sent(STATE_SC_SOCOPT);
				}
				else if(cc3000_is_ready()){
					printf("setsock: %i\n\r",cc3000_get_return_int8(0));
					cc3000_fh_set_active(STATE_SC_ACCEPT);
				}
				break;

			// accept socket
			case STATE_SC_ACCEPT:
				if(!_cc3000_fh_get_active_sent(STATE_SC_ACCEPT)){
					cc3000_req_accept(socket);
					_cc3000_fh_set_active_sent(STATE_SC_ACCEPT);
				}
				else if(cc3000_is_ready()){
					clientstatus = cc3000_get_accecpt(socket,(sockaddr *) &clientaddr, &addrlen);
					printf("Accept: %i\n\r",clientstatus);
					if((clientstatus == -1) || (clientstatus == -2)){
						cc3000_fh_set_active(STATE_SC_ACCEPT);
						_cc3000_fh_set_active_sent(STATE_IDLE);
					}else{
						clientIP[0] = clientaddr.sa_data[2];
						clientIP[1] = clientaddr.sa_data[3];
						clientIP[2] = clientaddr.sa_data[4];
						clientIP[3] = clientaddr.sa_data[5];

						printf("client: %u.%u.%u.%u connected\n\r",clientIP[3],clientIP[2],clientIP[1],clientIP[0]);
						cc3000_fh_set_active(STATE_SC_SELECT);
					}
				}
				break;


			// socket select
			case STATE_SC_SELECT:
				if(!_cc3000_fh_get_active_sent(STATE_SC_SELECT)){

					memset(&timeout, 0, sizeof(timeval));
					timeout.tv_sec = 0;//1;
					timeout.tv_usec = 50;//50*100;

					cc3000_FD_ZERO( &readsock );
					cc3000_FD_ZERO( &writesock );
					cc3000_FD_ZERO( &exceptsock );

					//Add client socket ID to to the read set
					cc3000_FD_SET(clientstatus, &readsock);
					cc3000_FD_SET(clientstatus, &writesock);
					cc3000_FD_SET(clientstatus, &exceptsock);
					maxFD = clientstatus + 1;

					cc3000_req_select(maxFD, &readsock, &writesock, &exceptsock, &timeout);
					_cc3000_fh_set_active_sent(STATE_SC_SELECT);
				}
				else if(cc3000_is_ready()){

					status = cc3000_get_select(&readsock, &writesock, &exceptsock);
					//printf("***SECELCT STATUS: %i***\n\r",status);
					if (FD_ISSET(clientstatus, &readsock)){
						// we can read
						cc3000_fh_set_active(STATE_SC_REC_REQ);
					}
					else if (FD_ISSET(clientstatus, &writesock)){
						// we can write
						if(counter<=1000){
							cc3000_fh_set_active(STATE_SC_TRX_DATA);
						}else{
							counter = 0;
							cc3000_fh_set_active(STATE_SC_CLOSE);
						}
					}
					else if (FD_ISSET(clientstatus, &exceptsock)){
						// we don't know what to do...
						printf("i don't know what a exceptsock is.. but we have one\n\r");
						_cc3000_fh_set_active_sent(STATE_IDLE);
						cc3000_fh_set_active(STATE_SC_SELECT);
					}
					else{
						_cc3000_fh_set_active_sent(STATE_IDLE);
						cc3000_fh_set_active(STATE_SC_SELECT);
					}
					// ToDo: this is unclean
					if(cc3000_close_socket){
						cc3000_close_socket = 0;
						cc3000_fh_set_active(STATE_SC_CLOSE);
					}
				}
				break;


			// receive
			case STATE_SC_REC_REQ:
				if(!_cc3000_fh_get_active_sent(STATE_SC_REC_REQ)){
					cc3000_req_recv(clientstatus,sizeof(recv_buffer),0);
					_cc3000_fh_set_active_sent(STATE_SC_REC_REQ);
				}
				// now we got the number of bytes
				else if(cc3000_is_ready()){
					recv_bytes = cc3000_get_recv_bytes();
					if(recv_bytes){
						//printf("there are %u bytes waiting..\n\r",recv_bytes);
						cc3000_fh_set_active(STATE_SC_REC_DATA);
					}else{
						cc3000_fh_set_active(STATE_IDLE);
					}
				}
				break;

			// receive data
			case STATE_SC_REC_DATA:
				if(cc3000_is_ready()){
					cc3000_get_recv_data(recv_buffer,recv_bytes);
					printf("RX: %s\n\r",recv_buffer);
					_cc3000_fh_set_active_sent(STATE_IDLE);
					cc3000_fh_set_active(STATE_SC_SELECT);
				}
				break;

			// send data
			case STATE_SC_TRX_DATA:
				if(!_cc3000_fh_get_active_sent(STATE_SC_TRX_DATA)){
					sprintf(tcp_tx_buff,"Ping: %u\n\r",counter);
					cc3000_send(clientstatus,tcp_tx_buff,strlen(tcp_tx_buff),0);
					printf("TX: %s\n\r",tcp_tx_buff);
					counter++;
					_cc3000_fh_set_active_sent(STATE_IDLE);
					cc3000_fh_set_active(STATE_SC_SELECT);
				}
				break;

			// close socket
			case STATE_SC_CLOSE:
				if(!_cc3000_fh_get_active_sent(STATE_SC_CLOSE)){
					cc3000_socket_close(clientstatus);
					_cc3000_fh_set_active_sent(STATE_SC_CLOSE);
				}else if(cc3000_is_ready()){
					debug_str("The socket is now closed\n");
					cc3000_fh_set_active(STATE_IDLE);
				}
				break;

			// set dhcp vaules
			case STATE_NA_DHCP:
				if(!_cc3000_fh_get_active_sent(STATE_NA_DHCP)){
					cc3000_netapp_dhcp(cc3000_aucIP, cc3000_subNet, cc3000_aucGateway, cc3000_aucDNSServer);
					_cc3000_fh_set_active_sent(STATE_NA_DHCP);
				}else if(cc3000_is_ready()){
					printf("netapp_dhcp: %u\n\r",cc3000_get_return_uint32(0));
					cc3000_fh_set_active(STATE_IDLE);
				}
				break;

			// delete all profiles
			case STATE_WL_DEL_P:
				if(!_cc3000_fh_get_active_sent(STATE_WL_DEL_P)){
					cc3000_wlan_del_profile(255);
					_cc3000_fh_set_active_sent(STATE_WL_DEL_P);
				}else if(cc3000_is_ready()){
					printf("del profile: %u\n\r",cc3000_get_return_uint32(0));
					cc3000_fh_set_active(STATE_IDLE);
				}
				break;

			// add profile
			case STATE_WL_ADD_P:
				if(!_cc3000_fh_get_active_sent(STATE_WL_ADD_P)){
					cc3000_wlan_add_profile(WLAN_SEC_WPA2,ssid,8,NULL,0,0x18, 0x1e, 0x2,key,8);
					_cc3000_fh_set_active_sent(STATE_WL_ADD_P);
				}else if(cc3000_is_ready()){
					printf("add profile: %u\n\r",cc3000_get_return_uint32(0));
					cc3000_fh_set_active(STATE_IDLE);
				}
				break;

			// power down
			// ToDO: we have to check first if we can powerdown (see event handler)
			case STATE_POWERDOWN:
				cc3000_clear_pin_WL_EN();
				cc3000_fh_set_active(STATE_IDLE);
				break;

		default:
			break;
	}
}


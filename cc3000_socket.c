/*
 * cc3000_socket.c
 *
 *  Created on: 09.09.2013
 *      Author: Johannes
 */

// ToDo: gethostbyname,getsockopt,

#include "cc3000_socket.h"
#include "cc3000_platform.h"
#include "cc3000_event_handler.h"
#include "cc3000_general.h"
#include "cc3000_hci.h"
#include <string.h>

uint32 socket_active_status = SOCKET_STATUS_INIT_VAL;
//*****************************************************************************
//
//!  set_socket_active_status
//!
//!  @param Sd
//!	 @param Status
//!  @return         none
//!
//!  @brief          Check if the socket ID and status are valid and set
//!                  accordingly  the global socket status
//
//*****************************************************************************
void set_socket_active_status(uint8 Sd, int32 Status){
	if(M_IS_VALID_SD(Sd) && M_IS_VALID_STATUS(Status)){
		socket_active_status &= ~(1 << Sd);      /* clean socket's mask */
		socket_active_status |= (Status << Sd); /* set new socket's mask */
	}
}
//*****************************************************************************
//
//!  get_socket_active_status
//!
//!  @param  Sd  Socket IS
//!  @return     Current status of the socket.
//!
//!  @brief  Retrieve socket status
//
//*****************************************************************************
int32 get_socket_active_status(uint8 Sd){
	if(M_IS_VALID_SD(Sd)){
		return (socket_active_status & (1 << Sd)) ? SOCKET_STATUS_INACTIVE : SOCKET_STATUS_ACTIVE;
	}
	return SOCKET_STATUS_INACTIVE;
}
//*****************************************************************************
//
//! cc3000_req_socket
//!
//!  @param  	domain 		only AF_INET is supported
//!	 @param		type		socket type
//!  @param		protocoll	socket protocoll (TCP or UDP etc.)
//!
//!  @return  	none
//!
//!  @brief  	request a socket
//!
//
//*****************************************************************************
void cc3000_req_socket(uint8 domain, uint8 type, uint8 protocol){

	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_SOCKET, 3*4);
	cc3000_hci_send_uint32(domain);
	cc3000_hci_send_uint32(type);
	cc3000_hci_send_uint32(protocol);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_SOCKET);
}
//*****************************************************************************
//
//! cc3000_connect
//!
//!  @param  	sd 			socket handler
//!	 @param		port		port to connect to
//!  @param		addr		address of the server
//!
//!  @return  	none
//!
//!  @brief  	connect to a socket
//!
//
//*****************************************************************************
void cc3000_connect(uint8 sd, uint16 port, ip_addr_t addr){

	uint32 addr32;
	addr32  =  addr.ip[0];
	addr32 += (addr.ip[1]<<8);
	addr32 += (addr.ip[2]<<16);
	addr32 += (addr.ip[3]<<24);

	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_CONNECT, 20);
    cc3000_hci_send_uint32(sd);
	cc3000_hci_send_uint32(0x00000008);	// magic
	cc3000_hci_send_uint32(8);	// addrlen
	cc3000_hci_send_uint16(AF_INET /*stSockAddr.sin_framily */);
	cc3000_hci_send_uint16(htons(port) /*stSockAddr.sin_port */);
	cc3000_hci_send_uint32(addr32 /*stSockAddr.sin_addr*/);
	cc3000_hci_end_command();

	cc3000_hci_expect(HCI_EVNT_CONNECT);

}
//*****************************************************************************
//
//! cc3000_req_accept
//!
//!  @param  	sd 			socket handler
//!
//!  @return  	none
//!
//!  @brief  	request an acception for a connection on a socket
//!
//
//*****************************************************************************
void cc3000_req_accept(uint8 sd){

	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_ACCEPT,4);
	cc3000_hci_send_uint32(sd);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_ACCEPT);
}
//*****************************************************************************
//
//! cc3000_get_accecpt
//!
//!  @param  	sd 			socket handler
//!	 @param		addr		address of the peer socket
//!  @param		addrlen		size of socket address
//!
//!  @return  	For socket in blocking mode:
//!				      On success, socket handle. on failure negative
//!			      For socket in non-blocking mode:
//!				     - On connection establishment, socket handle
//!				     - On connection pending, SOC_IN_PROGRESS (-2)
//!			       - On failure, SOC_ERROR	(-1)
//!
//!  @brief  	get the data of the connected client
//!
//
//*****************************************************************************
int32 cc3000_get_accecpt(uint8 sd, sockaddr *addr, socklen_t *addrlen){

	int32 ret;
	uint8 evt_ret[16];

	// ToDO: make this cleaner and figure the return parameter out
	// need specify return parameters!!!
	cc3000_get_return_stream(evt_ret,0,16);

	addr->sa_data[5] = evt_ret[15];
	addr->sa_data[4] = evt_ret[14];
	addr->sa_data[3] = evt_ret[13];
	addr->sa_data[2] = evt_ret[12];
	addr->sa_data[1] = evt_ret[11];
	addr->sa_data[0] = evt_ret[10];
	addr->sa_family = ((evt_ret[9]<<8)|evt_ret[8]);
	*addrlen = ASIC_ADDR_LEN;
	ret = cc3000_get_return_int32(4);

	// if succeeded, iStatus = new socket descriptor. otherwise - error number
	if(M_IS_VALID_SD(ret)){
		set_socket_active_status(ret, SOCKET_STATUS_ACTIVE);
	}else{
		set_socket_active_status(sd, SOCKET_STATUS_INACTIVE);
	}

	return(ret);

}
//*****************************************************************************
//
//! cc3000_listen
//!
//!  @param  	sd 			socket handler
//!
//!  @return  	none
//!
//!  @brief  	listen for connections on a socket
//!
//
//*****************************************************************************
void cc3000_listen(uint8 sd){
	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_LISTEN,8);
	cc3000_hci_send_uint32(sd);			//socket
	cc3000_hci_send_uint32(0x00000001);  //backlog
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_LISTEN);
}
//*****************************************************************************
//
//! cc3000_listen
//!
//!  @param  	sd 			socket handler
//!  @param  	port 		socket port
//!
//!  @return  	none
//!
//!  @brief  	assign a name to a socket
//!
//
//*****************************************************************************
void cc3000_bind(uint8 sd, uint16 port){
	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_BIND,20);
	cc3000_hci_send_uint32(sd);			//socket
	cc3000_hci_send_uint32(0x00000008);	//magic
	cc3000_hci_send_uint32(8);			//addrlen
	cc3000_hci_send_uint16(AF_INET);		//socket family
	cc3000_hci_send_uint16(htons(port)); //socket port
	cc3000_hci_send_uint32(0x00000000);	//IP
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_BIND);
}
//*****************************************************************************
//
//! cc3000_setsockopt
//!
//!  @param   sd          socket handle
//!  @param   level       defines the protocol level for this option
//!  @param   optname     defines the option name to Interrogate
//!  @param   optval      specifies a value for the option
//!  @param   optlen      specifies the length of the option value
//!  @return  	  none
//!
//!  @brief  set socket options
//
//*****************************************************************************
void cc3000_setsockopt(uint8 sd, uint32 level, uint32 optname, const void *optval, socklen_t optlen){

	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_SETSOCKOPT,22);
	cc3000_hci_send_uint32(sd);
	cc3000_hci_send_uint32(level);
	cc3000_hci_send_uint32(optname);
	cc3000_hci_send_uint32(0x00000008);
	cc3000_hci_send_uint32(optlen);
	cc3000_hci_send_uint16(optval);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_SETSOCKOPT);

}
//*****************************************************************************
//
//! cc3000_req_select
//!
//!  @param   	nfds       the highest-numbered file descriptor in any of the
//!                        three sets, plus 1.
//!  @param   	writesds   socket descriptors list for write monitoring
//!  @param   	readsds    socket descriptors list for read monitoring
//!  @param   	exceptsds  socket descriptors list for exception monitoring
//!  @param   	timeout    is an upper bound on the amount of time elapsed
//!                        before select() returns. Null means infinity
//!                        timeout. The minimum timeout is 5 milliseconds,
//!                        less than 5 milliseconds will be set
//!                        automatically to 5 milliseconds.
//!
//!  return			none
//!
//!  @brief  request monitoring of the socket
//
//*****************************************************************************
void cc3000_req_select(long nfds, cc3000_fd_set *readsds, cc3000_fd_set *writesds, cc3000_fd_set *exceptsds, struct timeval *timeout){

	unsigned long is_blocking;

	if( timeout == NULL){
		is_blocking = 1; /* blocking , infinity timeout */
	}else{
		is_blocking = 0; /* no blocking, timeout */
	}

	cc3000_desired_state++;

	cc3000_hci_start_command(HCI_CMND_BSD_SELECT,44);
	cc3000_hci_send_uint32(nfds);
	cc3000_hci_send_uint32(0x00000014);
	cc3000_hci_send_uint32(0x00000014);
	cc3000_hci_send_uint32(0x00000014);
	cc3000_hci_send_uint32(0x00000014);
	cc3000_hci_send_uint32(is_blocking);
	cc3000_hci_send_uint32((readsds) ? *(unsigned long*)readsds : 0);
	cc3000_hci_send_uint32((writesds) ? *(unsigned long*)writesds : 0);
	cc3000_hci_send_uint32((exceptsds) ? *(unsigned long*)exceptsds : 0);

	if(timeout){
		if ( 0 == timeout->tv_sec && timeout->tv_usec < SELECT_TIMEOUT_MIN_MICRO_SECONDS){
			timeout->tv_usec = SELECT_TIMEOUT_MIN_MICRO_SECONDS;
		}
		cc3000_hci_send_uint32(timeout->tv_sec);
		cc3000_hci_send_uint32(timeout->tv_usec);
	}else{
		cc3000_hci_send_uint32(0);
		cc3000_hci_send_uint32(0);
	}
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_SELECT);
}
//*****************************************************************************
//
//! cc3000_get_select
//!
//!  @param   	writesds   socket descriptors list for write monitoring
//!  @param   	readsds    socket descriptors list for read monitoring
//!  @param   	exceptsds  socket descriptors list for exception monitoring
//!
//!  return		On success, cc3000_get_select() returns the number of file descriptors
//!             contained in the three returned descriptor sets (that is, the
//!             total number of bits that are set in readfds, writefds,
//!             exceptfds) which may be zero if the timeout expires before
//!             anything interesting  happens.
//!             On error, -1 is returned.
//!
//!  @brief  request monitoring of the socket
//
//*****************************************************************************
int8 cc3000_get_select(cc3000_fd_set *readsds, cc3000_fd_set *writesds, cc3000_fd_set *exceptsds){

	int32 status;
	uint32 read;
	uint32 write;
	uint32 except;

	status = cc3000_get_return_int32(0);
	read = cc3000_get_return_uint32(4);
	write = cc3000_get_return_uint32(8);
	except = cc3000_get_return_uint32(12);

	// Update actually read FD
	if (status >= 0)
	{
		if (readsds){
			memcpy(readsds, &read, sizeof(read));
		}
		if (writesds){
			memcpy(writesds, &write, sizeof(write));
		}
		if (exceptsds){
			memcpy(exceptsds, &except, sizeof(except));
		}
		return(status);

	}
	else
	{
		return(-1);
	}
}
//*****************************************************************************
//
//!  cc3000_send
//!
//!  @param sd       socket handle
//!  @param buf      Points to a buffer containing the message to be sent
//!  @param len      message size in bytes
//!  @param flags    On this version, this parameter is not supported
//!
//!  @return         if executed returns 1, or -2 if an
//!                  error occurred
//!
//!  @brief          Write data to TCP socket
//!                  This function is used to transmit a message to another
//!                  socket.
//!
//
//*****************************************************************************
int8 cc3000_send(uint8 sd, uint8 *buf, uint16 len, uint8 flags){

uint16 count;

	if (cc3000_free_buffers == 0) {
		debug_str("Send: No free buffers!\n\r");
		return -2; // no free buffers;
	}

	cc3000_free_buffers--;	// strangely, we the host must keep track of free buffers

	cc3000_hci_start_data(HCI_CMND_SEND, 16, len);
	cc3000_hci_send_uint32(sd);
	cc3000_hci_send_uint32(16-4);
	cc3000_hci_send_uint32(len);
	cc3000_hci_send_uint32(flags);

	if (len > 0) {
		//debug_str("\n\rData:\n\r");
		for (count = 0; count < len; count++) {
			cc3000_hci_send_uint8(buf[count]);
			//debug_int_hex(buf[count]);
			//debug_putc(' ');
		}
	}
	cc3000_hci_end_data();

	return 1;
}
//*****************************************************************************
//
//!  cc3000_req_recv
//!
//!  @param  	sd     socket handle
//!  @param 	buf    Points to the buffer where the message should be stored
//!  @param  	len    Specifies the length in bytes of the buffer pointed to
//!                     by the buffer argument.
//!  @param 	flags   Specifies the type of message reception.
//!                     On this version, this parameter is not supported.
//!
//!  @return         none
//!
//!  @brief          function requests a message from a connection-mode socket
//!
//!
//
//*****************************************************************************
void cc3000_req_recv(uint8 sd, uint16 len, uint8 flags){

	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_RECV, 3*4);
	cc3000_hci_send_uint32(sd);
	cc3000_hci_send_uint32(len);
	cc3000_hci_send_uint32(flags);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_RECV);
}
//*****************************************************************************
//
//!  cc3000_req_recv
//!
//!  @param  	none
//!
//!  @return    returns number of bytes receives
//!
//!  @brief     gets the number of bytes received
//!
//!
//
//*****************************************************************************
int16 cc3000_get_recv_bytes(void){

	uint8 num_bytes;

	// In return, we get
	// int32 SocketDescriptor (8 bits will do us) - 5
	// int32 NumberOfBytes	(16 bits will do us)
	// int32 Flags

	// We will just assume that we are dealing with the socket we first thought of
	// In a more complex case of multiple sockets, we would need to separate this out
	// and probably have a more complex model (ie, a state machine)

	num_bytes = cc3000_get_return_uint8(4);

	if(num_bytes > 0){
		cc3000_desired_state++;
		cc3000_hci_expect(HCI_DATA_RECV);
	}

	return num_bytes;
}

//*****************************************************************************
//
//!  cc3000_get_recv_data
//!
//!  @param  	buf		pointer to receive buffer
//!
//!  @param  	len		number of bytes to receive - given by cc3000_get_recv_bytes
//!
//!  @return    none
//!
//!  @brief     gets the received data and stores it in buffer
//!
//!
//
//*****************************************************************************
void cc3000_get_recv_data(uint8 *buf, uint8 len){

	uint8 	args_length;

	args_length = cc3000_get_return_uint8(0);
	cc3000_get_return_stream(buf,args_length+3,len);
	buf[len] = '\0';

}
//*****************************************************************************
//
//! cc3000_socket_close
//!
//!  @param  sd    socket handle.
//!
//!  @return  On success, zero is returned. On error, -1 is returned.
//!
//!  @brief  The socket function closes a created socket.
//
//*****************************************************************************
void cc3000_socket_close(uint8 sd) {

	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_CLOSE_SOCKET, 4);
	cc3000_hci_send_uint32(sd);
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_CLOSE_SOCKET);
	set_socket_active_status(sd, SOCKET_STATUS_INACTIVE);
}
//*****************************************************************************
//
//!  mdnsAdvertiser
//!
//!  @param[in] mdnsEnabled         flag to enable/disable the mDNS feature
//!  @param[in] deviceServiceName   Service name as part of the published
//!                                 canonical domain name
//!
//!
//!  @return   On success, zero is returned, return SOC_ERROR if socket was not
//!            opened successfully, or if an error occurred.
//!
//!  @brief    Set CC3000 in mDNS advertiser mode in order to advertise itself.
//
//*****************************************************************************
int8 cc3000_socket_mdns_advertise(uint8 mdns_enabled, uint8 *service_name) {

	uint8 service_name_length;
	uint8 count;

	service_name_length = strlen(service_name);
	if (service_name_length > MDNS_DEVICE_SERVICE_MAX_LENGTH) {
		return -1;
	}
	cc3000_desired_state++;
	cc3000_hci_start_command(HCI_CMND_MDNS_ADVERTISE, 12 + service_name_length);
	cc3000_hci_send_uint32(mdns_enabled);
	cc3000_hci_send_uint32(8);	// magic
	cc3000_hci_send_uint32(service_name_length);
	for (count = 0; count < service_name_length; count++) {
		cc3000_hci_send_uint8(service_name[count]);
	}
	cc3000_hci_end_command();
	cc3000_hci_expect(HCI_EVNT_MDNS_ADVERTISE);

	//ToDO: do we return 1 or 0?
	return 0;
}

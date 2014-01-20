/*
 * cc3000_socket.h
 *
 *  Created on: 09.09.2013
 *      Author: Johannes
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CC3000_SOCKET_H_
#define CC3000_SOCKET_H_

#include "cc3000_platform.h"

// Socket domains / families
#define  AF_INET                2
#define  AF_INET6               23

// Socket type
#define  SOCK_STREAM            1
#define  SOCK_DGRAM             2
#define  SOCK_RAW               3           // Raw sockets allow new IPv4 protocols to be implemented in user space. A raw socket receives or sends the raw datagram not including link level headers
#define  SOCK_RDM               4
#define  SOCK_SEQPACKET         5

// Socket protocol
#define IPPROTO_IP              0           // dummy for IP
#define IPPROTO_ICMP            1           // control message protocol
#define IPPROTO_IPV4            IPPROTO_IP  // IP inside IP
#define IPPROTO_TCP             6           // tcp
#define IPPROTO_UDP             17          // user datagram protocol
#define IPPROTO_IPV6            41          // IPv6 in IPv6
#define IPPROTO_NONE            59          // No next header
#define IPPROTO_RAW             255         // raw IP packet

#define htons(a) a >> 8 | a << 8

// socket options - must find better define names for these. SOCK_ON is not representative of what it does!
#define  SOL_SOCKET             0xffff		//  socket level
#define  SOCKOPT_RECV_NONBLOCK         	0	// recv non block mode, set SOCK_ON or SOCK_OFF (default block mode)
#define  SOCKOPT_RECV_TIMEOUT			1	// optname to configure recv and recvfromtimeout
#define  SOCKOPT_ACCEPT_NONBLOCK		2	// accept non block mode, set SOCK_ON or SOCK_OFF (default block mode)
#define  SOCK_ON                		0	// socket non-blocking mode	is enabled
#define  SOCK_OFF               		1	// socket blocking mode is enabled


struct in_addr {
    uint32 s_addr;                 // load with inet_aton()
};

struct sockaddr_in {
    int16           sin_family;   ///< Socket family, e.g. AF_INET, AF_INET6
    uint16   		sin_port;     ///< Port, e.g. htons(3490)
    struct in_addr  sin_addr;     ///< Address, see struct in_addr, above
    int8            sin_zero[8];  ///< zero this if you want to, see http://silviocesare.wordpress.com/2007/10/22/setting-sin_zero-to-0-in-struct-sockaddr_in/
};

struct socket_accept_params_t
{
    int32            sd;
    int32            status;
    struct sockaddr_in		socket_address;

};

typedef struct _sockaddr_t
{
    uint16    sa_family;
    uint8     sa_data[14];
} sockaddr;

typedef struct _bsd_accept_return_t
{
    int32             iSocketDescriptor;
    int32             iStatus;
    sockaddr   		tSocketAddress;

} tBsdReturnParams;

typedef struct socket_select_params
{
    int32			status;
	uint32			read_sd;
	uint32 			write_sd;
	uint32 			ex_sd;
} tBsdSelectRecvParams;

typedef struct _bsd_getsockopt_return_t
{
	uint8			ucOptValue[4];
	int8			iStatus;
} tBsdGetSockOptReturnParams;

typedef struct _bsd_gethostbyname_return_t
{
    int32            retVal;
    int32            outputAddress;
} tBsdGethostbynameParams;

typedef struct _ip_addr_t {
	uint8 ip[4];
} ip_addr_t;

typedef struct timeval timeval;

struct timeval
{
    time_t         tv_sec;                  /* seconds */
    suseconds_t    tv_usec;                 /* microseconds */
};

// The cc3000_fd_set member is required to be an array of longs.
typedef int32 __fd_mask;
#define __cc3000_FD_SETSIZE      32
// It's easier to assume 8-bit bytes than to get CHAR_BIT.
#define __NFDBITS               (8 * sizeof (__fd_mask))
#define __FDELT(d)              ((d) / __NFDBITS)
#define __FDMASK(d)             ((__fd_mask) 1 << ((d) % __NFDBITS))

// cc3000_fd_set for select and pselect.
typedef struct
{
    __fd_mask fds_bits[__cc3000_FD_SETSIZE / __NFDBITS];
#define __FDS_BITS(set)        ((set)->fds_bits)
} cc3000_fd_set;

// We don't use `memset' because this would require a prototype and
//   the array isn't too big.
#define __cc3000_FD_ZERO(set)                               \
  do {                                                \
    unsigned int __i;                                 \
    cc3000_fd_set *__arr = (set);                            \
    for (__i = 0; __i < sizeof (cc3000_fd_set) / sizeof (__fd_mask); ++__i) \
      __FDS_BITS (__arr)[__i] = 0;                    \
  } while (0)


#define __cc3000_FD_SET(d, set)       (__FDS_BITS (set)[__FDELT (d)] |= __FDMASK (d))
#define __cc3000_FD_CLR(d, set)       (__FDS_BITS (set)[__FDELT (d)] &= ~__FDMASK (d))
#define __cc3000_FD_ISSET(d, set)     (__FDS_BITS (set)[__FDELT (d)] & __FDMASK (d))

// Access macros for 'cc3000_fd_set'.
#define cc3000_FD_SET(fd, fdsetp)      __cc3000_FD_SET (fd, fdsetp)
#define cc3000_FD_CLR(fd, fdsetp)      __cc3000_FD_CLR (fd, fdsetp)
#define cc3000_FD_ISSET(fd, fdsetp)    __cc3000_FD_ISSET (fd, fdsetp)
#define cc3000_FD_ZERO(fdsetp)         __cc3000_FD_ZERO (fdsetp)


typedef uint32 socklen_t;

#define MDNS_DEVICE_SERVICE_MAX_LENGTH 32
#define SOCKET_STATUS_ACTIVE       0
#define SOCKET_STATUS_INACTIVE     1

/* Init socket_active_status = 'all ones': init all sockets with SOCKET_STATUS_INACTIVE.
   Will be changed by 'set_socket_active_status' upon 'connect' and 'accept' calls */
#define SOCKET_STATUS_INIT_VAL  0xFFFF
#define M_IS_VALID_SD(sd) ((0 <= (sd)) && ((sd) <= 7))
#define M_IS_VALID_STATUS(status) (((status) == SOCKET_STATUS_ACTIVE)||((status) == SOCKET_STATUS_INACTIVE))

#define ASIC_ADDR_LEN          8
#define SELECT_TIMEOUT_MIN_MICRO_SECONDS  5000

extern uint32 socket_active_status;

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
extern void set_socket_active_status(uint8 Sd, int32 Status);
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
extern int32 get_socket_active_status(uint8 Sd);
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
extern void cc3000_req_socket(uint8 domain, uint8 type, uint8 protocol);
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
extern void cc3000_connect(uint8 sd, uint16 port, ip_addr_t addr);
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
extern void cc3000_req_accept(uint8 sd);
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
extern int32 cc3000_get_accecpt(uint8 sd, sockaddr *addr, socklen_t *addrlen);
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
extern void cc3000_listen(uint8 sd);
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
extern void cc3000_bind(uint8 sd, uint16 port);
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
extern void cc3000_setsockopt(uint8 sd, uint32 level, uint32 optname, const void *optval, socklen_t optlen);
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
extern void cc3000_req_select(long nfds, cc3000_fd_set *readsds, cc3000_fd_set *writesds, cc3000_fd_set *exceptsds, struct timeval *timeout);
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
extern int8 cc3000_get_select(cc3000_fd_set *readsds, cc3000_fd_set *writesds, cc3000_fd_set *exceptsds);
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
extern int8 cc3000_send(uint8 sd, uint8 *buf, uint16 len, uint8 flags);
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
extern void cc3000_req_recv(uint8 sd, uint16 len, uint8 flags);
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
extern int16 cc3000_get_recv_bytes(void);
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
extern void cc3000_get_recv_data(uint8 *buf, uint8 len);
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
extern void cc3000_socket_close(uint8 sd);
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
extern int8 cc3000_socket_mdns_advertise(uint8 mdns_enabled, uint8 *service_name);

#endif /* CC3000_SOCKET_H_ */

#ifdef __cplusplus
}
#endif


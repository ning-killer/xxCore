
#ifndef HTTP_CLIENT_WRAPPER
#define HTTP_CLIENT_WRAPPER


///////////////////////////////////////////////////////////////////////////////
//
// Section      : Microsoft Windows Support
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#pragma warning (disable: 4996) // 'function': was declared deprecated (VS 2005)
#include	<stdlib.h>
#include	<string.h>
#include	<memory.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<time.h>
#include	<winsock.h>



// Sockets (Winsock wrapper)
#define		HTTP_ECONNRESET     (WSAECONNRESET)
#define		HTTP_EINPROGRESS    (WSAEINPROGRESS)
#define		HTTP_EWOULDBLOCK    (WSAEWOULDBLOCK)

// Kluge alert: redefining strncasecmp() as memicmp() for Windows.
//
#define		strncasecmp			memicmp
#define		strcasecmp			stricmp

#else // Non Win32 : GCC Linux

#include	<unistd.h>
#include	<errno.h>
#include	<pthread.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<time.h>
#include    <sys/types.h>
#include	<sys/socket.h>
#include	<sys/un.h>
#include	<netinet/in.h>
#include	<netinet/tcp.h>
#include	<netdb.h>
#include	<arpa/inet.h>
#include	<sys/ioctl.h>
#include	<errno.h>
#include	<stdarg.h>
#include    <stdint.h>


#include "mbedtls/net.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include <errno.h>
#include <signal.h>

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define		SOCKET_ERROR			-1

// Sockets (Winsock wrapper)
#define		HTTP_EINPROGRESS    (EINPROGRESS)
#define		HTTP_EALREADY    (EALREADY)
#define		HTTP_EWOULDBLOCK    (EWOULDBLOCK)
// Generic types
typedef uint32_t                       UINT32;
typedef int32_t                        INT32;

#endif	// #ifdef _HTTP_BUILD_WIN32

// Note: define this to prevent timeouts while debugging.
// #define							 NO_TIMEOUTS

///////////////////////////////////////////////////////////////////////////////
//
// Section      : Functions that are not supported by the AMT stdc framework
//                So they had to be specificaly added.
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct
    {


        mbedtls_net_context         ssl_fd;
		mbedtls_net_context         ssl_fd4;
		#ifdef HTTPCLIENT_WITH_IPV6
		mbedtls_net_context         ssl_fd6;
		#endif
        mbedtls_entropy_context     entropy;
        mbedtls_ctr_drbg_context    ctr_drbg;
        mbedtls_ssl_context         ssl;
        mbedtls_ssl_config          conf;
        mbedtls_x509_crt            cacert;
        int timeout; //定义ssl连接timeout时间

    } HTTP_SSL;


    // STDC Wrapper implimentation
    int                                 HTTPWrapperIsAscii              (int c);
    int                                 HTTPWrapperToUpper              (int c);
    int                                 HTTPWrapperToLower              (int c);
    int                                 HTTPWrapperIsAlpha              (int c);
    int                                 HTTPWrapperIsAlNum              (int c);
    char*                               HTTPWrapperItoa                 (char *buff,int i);
    void                                HTTPWrapperInitRandomeNumber    (void);
    int64_t                                HTTPWrapperGetUpTime            (void);
    int                                 HTTPWrapperGetRandomeNumber     (void);
    int                                 HTTPWrapperGetSocketError       (int s);
    int                       HTTPWrapperGetHostByName        (char *name, UINT32 *address);
    int                                 HTTPWrapperShutDown             (int s,int in);
    // SSL Wrapper prototypes
    int HTTPWrapperSSLInit(HTTP_SSL *tls);

    int HTTPWrapperSSLConnect(HTTP_SSL *tls,int ipv4_have,struct sockaddr_in *serversockaddr, int ipv6_have,struct sockaddr_in6 *serversockaddr6,int bind_client_port, int falg, char *domain);
    int HTTPWrapperSSLNegotiate(HTTP_SSL *tls);
    int HTTPWrapperSSLSend(HTTP_SSL *tls,char *buffer, int len);
    int HTTPWrapperSSLRecv(HTTP_SSL *tls,char *buffer, int len );
    int HTTPWrapperSSLClose(HTTP_SSL *tls);
    int64_t HTTPWrapperSetMSS(int fd);

    // Global wrapper Functions
#define                             IToA                            HTTPWrapperItoa
#define                             GetUpTime                       HTTPWrapperGetUpTime
#define                             SocketGetErr                    HTTPWrapperGetSocketError
#define                             HostByName                      HTTPWrapperGetHostByName
#define                             InitRandomeNumber               HTTPWrapperInitRandomeNumber
#define                             GetRandomeNumber                HTTPWrapperGetRandomeNumber

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Section      : Global type definitions
// Last updated : 01/09/2005
//
///////////////////////////////////////////////////////////////////////////////

#define VOID                         void
#ifndef NULL
#define NULL                         0
#endif

#ifdef __LITEOS__
#else
#define TRUE                         1
#define FALSE                        0
#endif

typedef char                         CHAR;
typedef unsigned short               UINT16;
#ifdef __LITEOS__
#else\

#ifndef BOOL
typedef int                          BOOL;
#endif
#endif
//typedef unsigned long                ULONG;

// Global socket structures and definitions
#define                              HTTP_INVALID_SOCKET (-1)
typedef struct sockaddr_in           HTTP_SOCKADDR_IN;
typedef struct sockaddr_in6           HTTP_SOCKADDR_IN6;

typedef struct timeval               HTTP_TIMEVAL;
typedef struct hostent               HTTP_HOSTNET;
typedef struct sockaddr              HTTP_SOCKADDR;
typedef struct in_addr               HTTP_INADDR;




#endif // HTTP_CLIENT_WRAPPER

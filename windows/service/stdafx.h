#ifndef __STDAFX_H__
#define __STDAFX_H__


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#ifdef VXWORKS

#include <vxWorks.h>	/* Must be first */
#include <inetLib.h>
#include <sockLib.h>
#include <taskLib.h>
#include <vxwMutex.h>
#include <ioLib.h>
#include <memLib.h>
#include <loadLib.h>
#include <sysLib.h>
typedef unsigned long long uint64_t;
typedef long long int64_t;

#endif /* VXWORKS */



#ifdef WIN32

	typedef unsigned char uint8_t;
	typedef char int8_t;

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifdef __cplusplus

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
#include <sys/stat.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <io.h>

#pragma comment( lib, "Ws2_32.lib" )


/* int8_t的定义在LINUX上易造成编译错误，干脆不用int8_t,uint8_t */
//typedef unsigned char uint8_t;
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned __int64 uint64_t;
typedef short int16_t;
//typedef signed char int8_t;
typedef int int32_t;
typedef __int64 int64_t;
typedef unsigned int size_t;

#endif /* WIN32 */


#ifdef LINUX

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <signal.h>
#include <wchar.h>
#include <fcntl.h>
#include <getopt.h>
#include <termios.h>
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <net/if.h> 
#include <net/if_arp.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/param.h> 
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <utime.h>
#include <sys/reboot.h>

typedef unsigned char BYTE;

#endif /* LINUX */


#ifndef __cplusplus

#ifndef bool
#define bool uint8_t
#endif	

#ifndef true
#define true 1
#endif	

#ifndef false
#define false 0
#endif 

#endif /* __cplusplus */

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef INVALID_PCR
	#define INVALID_PCR		0xffffffff
#endif

#ifdef _DEBUG
	#define OCG_ASSERT(flag)	os_sys_assert(flag, __FILE__, __LINE__)
#else
	#define OCG_ASSERT(flag)
#endif


#endif /* __STDAFX_H__ */

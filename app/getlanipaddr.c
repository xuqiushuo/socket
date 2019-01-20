/**
  *需要的头文件
  *
  *直接调用lanGetNetIpAddr函数，传入接口名（如eth0）等参数
  */
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

static int lanGetNetIpAddr(const char *ifName, char *ipAddr, UINT32 addrLen)
{
    int ret = -1;
    struct ifreq ifr;
    struct sockaddr_in *sin;
    int sock;

    printf("ifname:%s", ifr.ifr_name);
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(ifr.ifr_name, ifName, sizeof(ifr.ifr_name));

    if (0 > (ret = ioctl(sock, SIOCGIFADDR, &ifr)))
    {
        printf("ioctl get SIOCGIFADDR failed!!");
        return -1;
    }
    close(sock);

    sin = (struct sockaddr_in *)&(ifr.ifr_addr);
    if (NULL == inet_ntoa(sin->sin_addr))
    {
        return -1;
    }
    strncpy(ipAddr, inet_ntoa(sin->sin_addr), addrLen);

    return ret;
}

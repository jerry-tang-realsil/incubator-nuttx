/****************************************************************************
 * include/nuttx/net/ip.h
 *
 * This header file contains IP-related definitions for a number of C
 * macros that are used by applications as well as internally by the
 * OS networking logic.
 *
 *   Copyright (C) 2007-2012, 2014 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * This logic was leveraged from uIP which also has a BSD-style license:
 *
 *   Author Adam Dunkels <adam@dunkels.com>
 *   Copyright (c) 2001-2003, Adam Dunkels.
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __INCLUDE_NUTTX_NET_IP_H
#define __INCLUDE_NUTTX_NET_IP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <queue.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <nuttx/net/netconfig.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/* IP Version Mask (bits 0-3 of first byte) */

#define IP_VERSION_MASK  0x70
#define IPv4_VERSION     0x40
#define IPv6_VERSION     0x60

/* Values for the IP protocol field */

#define IP_PROTO_ICMP     1
#define IP_PROTO_IGMP     2
#define IP_PROTO_TCP      6
#define IP_PROTO_UDP      17
#define IP_PROTO_ICMP6    58

/* Flag bits in 16-bit flags + fragment offset IPv4 header field */

#define IP_FLAG_RESERVED  0x8000
#define IP_FLAG_DONTFRAG  0x4000
#define IP_FLAG_MOREFRAGS 0x2000

/* IP Header sizes */

#ifdef CONFIG_NET_IPv4
#  define IPv4_HDRLEN     20    /* Size of IPv4 header */
#endif

#ifdef CONFIG_NET_IPv6
#  define IPv6_HDRLEN     40    /* Size of IPv6 header */
#endif

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

/* Representation of an IP address in the IPv6 domains */

typedef uint16_t net_ipv6addr_t[8];

#if defined(CONFIG_NET_IPv4) || defined(CONFIG_NET_IPv6)
/* Describes and address in either the IPv4 or IPv6 domain */

union ip_addr_u
{
#ifdef CONFIG_NET_IPv4
  /* IPv4 address */

  in_addr_t ipv4;
#endif

#ifdef CONFIG_NET_IPv6
  /* IPv6 address */

  net_ipv6addr_t ipv6;
#endif
};
#endif

#if defined(CONFIG_NET_IPv4) || defined(CONFIG_NET_IPv6)
/* Describes address binding for a PF_INET or PF_INET6 socket */

union ip_binding_u
{
#ifdef CONFIG_NET_IPv4
  /* IPv4 addresses (for PF_INET socket) */

  struct
  {
#ifdef CONFIG_NETDEV_MULTINIC
    in_addr_t laddr;      /* The bound local IPv4 address */
#endif
    in_addr_t raddr;      /* The IPv4 address of the remote host */
  } ipv4;
#endif /* CONFIG_NET_IPv4 */

#ifdef CONFIG_NET_IPv6
  /* IPv6 addresses (for PF_INET6 socket) */

  struct
  {
#ifdef CONFIG_NETDEV_MULTINIC
    net_ipv6addr_t laddr; /* The bound local IPv6 address */
#endif
    net_ipv6addr_t raddr; /* The IPv6 address of the remote host */
  } ipv6;
#endif /* CONFIG_NET_IPv6 */
};
#endif

#ifdef CONFIG_NET_IPv4
/* The IPv4 header */

struct ipv4_hdr_s
{
  uint8_t  vhl;              /*  8-bit Version (4) and header length (5 or 6) */
  uint8_t  tos;              /*  8-bit Type of service (e.g., 6=TCP) */
  uint8_t  len[2];           /* 16-bit Total length */
  uint8_t  ipid[2];          /* 16-bit Identification */
  uint8_t  ipoffset[2];      /* 16-bit IP flags + fragment offset */
  uint8_t  ttl;              /*  8-bit Time to Live */
  uint8_t  proto;            /*  8-bit Protocol */
  uint16_t ipchksum;         /* 16-bit Header checksum */
  uint16_t srcipaddr[2];     /* 32-bit Source IP address */
  uint16_t destipaddr[2];    /* 32-bit Destination IP address */
};
#endif /* CONFIG_NET_IPv4 */

#ifdef CONFIG_NET_IPv6
/* The IPv6 header */

struct ipv6_hdr_s
{
  uint8_t  vtc;              /* Bits 0-3: version, bits 4-7: traffic class (MS) */
  uint8_t  tcf;              /* Bits 0-3: traffic class (LS), 4-bits: flow label (MS) */
  uint16_t flow;             /* 16-bit flow label (LS) */
  uint8_t  len[2];           /* 16-bit Payload length */
  uint8_t  proto;            /*  8-bit Next header (same as IPv4 protocol field) */
  uint8_t  ttl;              /*  8-bit Hop limit (like IPv4 TTL field) */
  net_ipv6addr_t srcipaddr;  /* 128-bit Source address */
  net_ipv6addr_t destipaddr; /* 128-bit Destination address */
};
#endif /* CONFIG_NET_IPv6 */

#ifdef CONFIG_NET_STATISTICS
#ifdef CONFIG_NET_IPv4
struct ipv4_stats_s
{
  net_stats_t drop;       /* Number of dropped packets at the IP layer */
  net_stats_t recv;       /* Number of received packets at the IP layer */
  net_stats_t sent;       /* Number of sent packets at the IP layer */
  net_stats_t vhlerr;     /* Number of packets dropped due to wrong
                             IP version or header length */
  net_stats_t fragerr;    /* Number of packets dropped since they
                             were IP fragments */
  net_stats_t chkerr;     /* Number of packets dropped due to IP
                             checksum errors */
  net_stats_t protoerr;   /* Number of packets dropped since they
                             were neither ICMP, UDP nor TCP */
};
#endif /* CONFIG_NET_IPv6 */

#ifdef CONFIG_NET_IPv6
struct ipv6_stats_s
{
  net_stats_t drop;       /* Number of dropped packets at the IP layer */
  net_stats_t recv;       /* Number of received packets at the IP layer */
  net_stats_t sent;       /* Number of sent packets at the IP layer */
  net_stats_t vhlerr;     /* Number of packets dropped due to wrong
                             IP version or header length */
  net_stats_t protoerr;   /* Number of packets dropped since they
                             were neither ICMP, UDP nor TCP */
};
#endif /* CONFIG_NET_IPv6 */
#endif /* CONFIG_NET_STATISTICS */

/****************************************************************************
 * Public Data
 ****************************************************************************/
#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/* Well-known IP addresses */

#ifdef CONFIG_NET_IPv6
EXTERN const net_ipv6addr_t g_ipv6_alloneaddr;  /* An address of all ones */
EXTERN const net_ipv6addr_t g_ipv6_allzeroaddr; /* An address of all zeroes */
#if defined(CONFIG_NET_ICMPv6_AUTOCONF) || defined(CONFIG_NET_ICMPv6_ROUTER)
EXTERN const net_ipv6addr_t g_ipv6_allnodes;    /* All link local nodes */
EXTERN const net_ipv6addr_t g_ipv6_allrouters;  /* All link local routers */
#ifdef CONFIG_NET_ICMPv6_AUTOCONF
EXTERN const net_ipv6addr_t g_ipv6_llnetmask;   /* Netmask for local link address */
#endif
#endif
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Macro: net_ipaddr
 *
 * Description:
 *   Construct an IPv4 address from four bytes.
 *
 *   This function constructs an IPv4 address in network byte order.
 *
 * Input Parameters:
 *   addr  A pointer to a in_addr_t variable that will be
 *         filled in with the IPv4 address.
 *   addr0 The first octet of the IPv4 address.
 *   addr1 The second octet of the IPv4 address.
 *   addr2 The third octet of the IPv4 address.
 *   addr3 The forth octet of the IPv4 address.
 *
 ****************************************************************************/

#define net_ipaddr(addr, addr0, addr1, addr2, addr3) \
  do { \
    addr = HTONL((uint32_t)(addr0) << 24 | (uint32_t)(addr1) << 16 | \
                 (uint32_t)(addr2) << 8  | (uint32_t)(addr3)); \
  } while (0)

/****************************************************************************
 * Macro: net_ip4addr_conv32
 *
 * Description:
 *   Convert an IPv4 address of the form uint16_t[2] to an in_addr_t
 *
 ****************************************************************************/

#ifdef CONFIG_ENDIAN_BIG
#  define net_ip4addr_conv32(addr) \
    (((in_addr_t)((uint16_t*)addr)[0] << 16) | \
     (in_addr_t)((uint16_t*)addr)[1])
#else
#  define net_ip4addr_conv32(addr) \
    (((in_addr_t)((uint16_t*)addr)[1] << 16) | \
     (in_addr_t)((uint16_t*)addr)[0])
#endif

/****************************************************************************
 * Macro: ip4_addr1, ip4_addr2, ip4_addr3, and ip4_addr4
 *
 * Description:
 *   Extract individual bytes from a 32-bit IPv4 IP address that is in
 *   network byte order.
 *
 ****************************************************************************/

#ifdef CONFIG_ENDIAN_BIG
   /* Big-endian byte order: 11223344 */

#  define ip4_addr1(ipaddr) (((ipaddr) >> 24) & 0xff)
#  define ip4_addr2(ipaddr) (((ipaddr) >> 16) & 0xff)
#  define ip4_addr3(ipaddr) (((ipaddr) >>  8) & 0xff)
#  define ip4_addr4(ipaddr)  ((ipaddr)        & 0xff)
#else
   /* Little endian byte order: 44223311 */

#  define ip4_addr1(ipaddr)  ((ipaddr)        & 0xff)
#  define ip4_addr2(ipaddr) (((ipaddr) >>  8) & 0xff)
#  define ip4_addr3(ipaddr) (((ipaddr) >> 16) & 0xff)
#  define ip4_addr4(ipaddr) (((ipaddr) >> 24) & 0xff)
#endif

/****************************************************************************
 * Macro: ip6_addr
 *
 * Description:
 *   Construct an IPv6 address from eight 16-bit words.
 *
 ****************************************************************************/

#define ip6_addr(addr, addr0,addr1,addr2,addr3,addr4,addr5,addr6,addr7) \
  do { \
    ((uint16_t*)(addr))[0] = HTONS((addr0)); \
    ((uint16_t*)(addr))[1] = HTONS((addr1)); \
    ((uint16_t*)(addr))[2] = HTONS((addr2)); \
    ((uint16_t*)(addr))[3] = HTONS((addr3)); \
    ((uint16_t*)(addr))[4] = HTONS((addr4)); \
    ((uint16_t*)(addr))[5] = HTONS((addr5)); \
    ((uint16_t*)(addr))[6] = HTONS((addr6)); \
    ((uint16_t*)(addr))[7] = HTONS((addr7)); \
  } while (0)

/****************************************************************************
 * Macro: ip6_map_ipv4addr
 *
 * Description:
 *   Hybrid dual-stack IPv6/IPv4 implementations recognize a special class
 *   of addresses, the IPv4-mapped IPv6 addresses. These addresses consist
 *   of:
 *
 *     1. An 80-bit prefix of zeros,
 *     2. The next 16 bits are one, and
 *     3. The remaining, least-significant 32 bits contain the IPv4 address.
 *
 *   This macro encodes an IPv4 address in an IPv6 address in this fashion.
 *
 * Input Parameters:
 *   ipv4addr - The IPv4 address to be mapped (scalar)
 *   ipv6addr - The IPv6 address in which to map the IPv4 address (array)
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

#define ip6_map_ipv4addr(ipv4addr, ipv6addr) \
  do \
    { \
      memset(ipv6addr, 0, 5 * sizeof(uint16_t)); \
      ipv6addr[5] = 0xffff; \
      ipv6addr[6] = (uint16_t)((uint32_t)ipv4addr >> 16); \
      ipv6addr[7] = (uint16_t)ipv4addr & 0xffff; \
    } \
  while (0)

/****************************************************************************
 * Macro: ip6_get_ipv4addr
 *
 * Description:
 *   Decode an IPv4-mapped IPv6 address.
 *
 * Input Parameters:
 *   ipv6addr - The IPv6 address (net_ipv6addr_t array) containing the mapped
 *     IPv4 address
 *
 * Returned Value:
 *   The decoded IPv4 address (scalar in_addr_t)
 *
 ****************************************************************************/

#define ip6_get_ipv4addr(ipv6addr) \
  (((in_addr_t)(ipv6addr)->s6_addr[12]) | \
   ((in_addr_t)(ipv6addr)->s6_addr[13] << 8) | \
   ((in_addr_t)(ipv6addr)->s6_addr[14] << 16) | \
   ((in_addr_t)(ipv6addr)->s6_addr[15] << 24))

/****************************************************************************
 * Macro: ip6_is_ipv4addr
 *
 * Description:
 *   Test if an IPv6 is an IPv4-mapped IPv6 address.
 *
 * Input Parameters:
 *   ipv6addr - The IPv6 address to be tested
 *
 * Returned Value:
 *   True is returned if ipv6addr holds a mapped IPv4 address.
 *
 ****************************************************************************/

#define ip6_is_ipv4addr(ipv6addr) \
  ((ipv6addr)->s6_addr32[0] == 0 && \
   (ipv6addr)->s6_addr32[1] == 0 && \
   (ipv6addr)->s6_addr16[4] == 0 && \
   (ipv6addr)->s6_addr16[5] == 0xffff)

/****************************************************************************
 * Macro: net_ipv4addr_copy, net_ipv4addr_hdrcopy, net_ipv6addr_copy, and
 *        net_ipv6addr_hdrcopy
 *
 * Description:
 *   Copy an IP address from one place to another.
 *
 * Example:
 *
 *   in_addr_t ipaddr1, ipaddr2;
 *
 *   net_ipaddr(&ipaddr1, 192,16,1,2);
 *   net_ipaddr_copy(&ipaddr2, &ipaddr1);
 *
 * Input Parameters:
 *   dest - The destination for the copy.
 *   src  - The source from where to copy.
 *
 ****************************************************************************/

#ifdef CONFIG_NET_IPv4
#  define net_ipv4addr_copy(dest, src) \
   do { \
     (dest) = (in_addr_t)(src); \
   } while (0)
#  define net_ipv4addr_hdrcopy(dest, src) \
   do { \
     ((uint16_t*)(dest))[0] = ((uint16_t*)(src))[0]; \
     ((uint16_t*)(dest))[1] = ((uint16_t*)(src))[1]; \
   } while (0)
#endif

#ifdef CONFIG_NET_IPv6
#  define net_ipv6addr_copy(dest,src) \
   memcpy(dest, src, sizeof(net_ipv6addr_t))
#  define net_ipv6addr_hdrcopy(dest,src) \
   net_ipv6addr_copy(dest, src)
#endif

/****************************************************************************
 * Macro: net_ipv4addr_cmp, net_ipv4addr_hdrcmp, net_ipv6addr_cmp, and
 *        net_ipv6addr_hdrcmp
 *
 * Description:
 *   Compare two IP addresses
 *
 *   Example:
 *
 *     in_addr_t ipaddr1, ipaddr2;
 *
 *     net_ipaddr(&ipaddr1, 192,16,1,2);
 *     if (net_ipv4addr_cmp(ipaddr2, ipaddr1))
 *       {
 *         printf("They are the same");
 *       }
 *
 * Input Parameters:
 *   addr1 - The first IP address.
 *   addr2 - The second IP address.
 *
 ****************************************************************************/

#ifdef CONFIG_NET_IPv4
#  define net_ipv4addr_cmp(addr1, addr2) \
  (addr1 == addr2)
#  define net_ipv4addr_hdrcmp(addr1, addr2) \
   net_ipv4addr_cmp(net_ip4addr_conv32(addr1), net_ip4addr_conv32(addr2))
#endif

#ifdef CONFIG_NET_IPv6
#  define net_ipv6addr_cmp(addr1, addr2) \
   (memcmp(addr1, addr2, sizeof(net_ipv6addr_t)) == 0)
#  define net_ipv6addr_hdrcmp(addr1, addr2) \
   net_ipv6addr_cmp(addr1, addr2)
#endif

/****************************************************************************
 * Function: net_ipv4addr_maskcmp and net_ipv6addr_maskcmp
 *
 * Description:
 *   Compare two IP addresses under a netmask.  The mask is used to mask
 *   out the bits that are to be compared:  Buts within the mask much
 *   match exactly; bits outside if the mask are ignored.
 *
 * IPv4 Example:
 *
 *   net_ipv6addr_t ipaddr1;
 *   net_ipv6addr_t ipaddr2;
 *   net_ipv6addr_t mask;
 *
 *   net_ipv6addr(&mask, 255,255,255,0);
 *   net_ipv6addr(&ipaddr1, 192,16,1,2);
 *   net_iv6paddr(&ipaddr2, 192,16,1,3);
 *   if (net_ipv6addr_maskcmp(ipaddr1, ipaddr2, &mask))
 *     {
 *       printf("They are the same");
 *     }
 *
 * Input Parameters:
 *   addr1 - The first IP address.
 *   addr2 - The second IP address.
 *   mask  - The netmask.
 *
 * Returned Value:
 *   True if the address under the mask are equal
 *
 ****************************************************************************/

#ifdef CONFIG_NET_IPv4
#  define net_ipv4addr_maskcmp(addr1, addr2, mask) \
   (((in_addr_t)(addr1) & (in_addr_t)(mask)) == \
    ((in_addr_t)(addr2) & (in_addr_t)(mask)))
#endif

#ifdef CONFIG_NET_IPv6
bool net_ipv6addr_maskcmp(const net_ipv6addr_t addr1,
                          const net_ipv6addr_t addr2,
                          const net_ipv6addr_t mask);
#endif

/****************************************************************************
 * Function: net_ipv6addr_prefixcmp
 *
 * Description:
 *   Compare two IPv6 address prefixes.
 *
 ****************************************************************************/

#define net_ipv6addr_prefixcmp(addr1, addr2, length) \
  (memcmp(addr1, addr2, length >> 3) == 0)

/****************************************************************************
 * Function: net_ipaddr_mask
 *
 * Description:
 *    Mask out the network part of an IP address, given the address and
 *    the netmask.
 *
 *    Example:
 *
 *     in_addr_t ipaddr1, ipaddr2, netmask;
 *
 *     net_ipaddr(&ipaddr1, 192,16,1,2);
 *     net_ipaddr(&netmask, 255,255,255,0);
 *     net_ipaddr_mask(&ipaddr2, &ipaddr1, &netmask);
 *
 *   In the example above, the variable "ipaddr2" will contain the IP
 *   address 192.168.1.0.
 *
 * Input Parameters:
 *   dest Where the result is to be placed.
 *   src The IP address.
 *   mask The netmask.
 *
 ****************************************************************************/

#define net_ipaddr_mask(dest, src, mask) \
  do { \
    (in_addr_t)(dest) = (in_addr_t)(src) & (in_addr_t)(mask); \
  } while (0)

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_NUTTX_NET_IP_H */

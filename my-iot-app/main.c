/*
    Application for sending temperature and ambient light data with UDP to a server with IPv6 address
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "msg.h"
#include "luid.h"
#include "ztimer.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/pktdump.h"

#include "temperature.h"
#include "pressure.h"
#include "lineprotocol.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#define ID_LEN_BYTES 12
#define LUID_FMT_STRING "%02x%02x%02x%02x-%02x%02x%02x%02x-%02x%02x%02x%02x"
#define LUID_MAGIC 0x2023

#define TARGET_IPV6_ADDR "2a05:d016:bb1:9a01:fcaf:16c1:d73f:d5ab"
#define TARGET_IPV6_PORT 50123

static char raw_id[ID_LEN_BYTES];
static char hex_id[(2*ID_LEN_BYTES) + 3]; // two characters per byte + two separators + null at the end

gnrc_netif_t *netif = NULL;
ipv6_addr_t addr;

// typedef int (*shell_command_handler_t)(int argc, char **argv);
int print_id(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("%s\n", hex_id);

    return 0;
}

int print_temperature(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int16_t temp = 0;
    if (read_raw_temperature(&temp) == TEMP_OP_NOK) {
        puts("failed to read temperature!");
        return 1;
    }

    printf("temperature is %d.%d°C\n", (temp / 100), (temp % 100));

    return 0;
}

int print_pressure(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint32_t pres = 0;
    if (read_raw_pressure(&pres) == PRESSURE_OP_NOK) {
        puts("failed to read air pressure!");
        return 1;
    }

    printf("air pressure is %ld Pa\n", pres);

    return 0;
}

int test_lineproto(int argc, char **argv) {
    (void)argc;
    (void)argv;

    int16_t temp = 0;
    uint32_t pres = 0;

    read_raw_temperature(&temp);
    read_raw_pressure(&pres);

    char buf[100];
    (void)memset(buf, 0, sizeof(buf));
    into_lineprotocol(buf, sizeof(buf), hex_id, &temp, &pres);

    printf("%s\n", buf);
    return 0;
}

int send_udp_packet(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    char data[100];
    int16_t temp = 0;
    uint32_t pres = 0;

    read_raw_temperature(&temp);
    read_raw_pressure(&pres);

    (void)memset(data, 0, sizeof(data));
    into_lineprotocol(data, sizeof(data), hex_id, &temp, &pres);
    uint16_t port = TARGET_IPV6_PORT;
    gnrc_pktsnip_t *payload, *udp, *ip;
    unsigned payload_size;
    
    /* allocate payload */
    payload = gnrc_pktbuf_add(NULL, data, strlen(data), GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        puts("Error: unable to copy data to packet buffer");
        return 1;
    }
    /* store size for output */
    payload_size = (unsigned)payload->size;
    /* allocate UDP header, set source port := destination port */
    udp = gnrc_udp_hdr_build(payload, port, port);
    if (udp == NULL) {
        puts("Error: unable to allocate UDP header");
        gnrc_pktbuf_release(payload);
        return 1;
    }
    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, &addr);
    if (ip == NULL) {
        puts("Error: unable to allocate IPv6 header");
        gnrc_pktbuf_release(udp);
        return 1;
    }
    /* add netif header, if interface was given */
    if (netif) {
        gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);

        ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = netif->pid;
        LL_PREPEND(ip, netif_hdr);
    }
    /* send packet */
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
        puts("Error: unable to locate UDP thread");
        gnrc_pktbuf_release(ip);
        return 1;
    }
    /* access to `payload` was implicitly given up with the send operation above
        * => use temporary variable for output */
    printf("Success: sent %u byte(s) to [%s]:%u\n", payload_size, TARGET_IPV6_ADDR,
            port);
    
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "id", "Print ID", print_id },
    { "temp", "Print temperature from sensor", print_temperature },
    { "pres", "Print air pressure from sensor", print_pressure },
    { "line", "Print current readings in lineprotocol format", test_lineproto },
    { "udp_send", "Send UDP packet to an endpoint. Usage: <addr / dns name> <port> <payload>", send_udp_packet },
    { NULL, NULL, NULL }
};

int main(void)
{
    (void)memset(raw_id, 0, sizeof(raw_id));
    (void)memset(hex_id, 0, sizeof(hex_id));

    // init temperature sensor
    if (init_temperature() == TEMP_OP_NOK) {
        puts("failed to init temperature sensor!");
        return 1;
    }

    // init pressure sensor
    if (init_pressure() == PRESSURE_OP_NOK) {
        puts("failed to init pressure sensor!");
        return 1;
    }

    // generate a unique-ish ID
    luid_custom(raw_id, ID_LEN_BYTES, LUID_MAGIC);
    sprintf(
        hex_id, 
        LUID_FMT_STRING, 
        raw_id[0], raw_id[1], raw_id[2],  raw_id[3], 
        raw_id[4], raw_id[5], raw_id[6],  raw_id[7], 
        raw_id[8], raw_id[9], raw_id[10], raw_id[11]
    );

    printf("My ID is %s\n",hex_id);

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("Network should be up!");

    // get a network interface to work with
    netif = gnrc_netif_iter(NULL);
    
    // parse target IP address
    if (ipv6_addr_from_str(&addr, TARGET_IPV6_ADDR) == NULL) {
        puts("failed to parse destination address");
        return 1;
    }

    /* start shell */
    /*
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    */
    (void)shell_commands;

    for (;;) {
        // call the "udp_send" handler to send data
        // badly named, i know :)
        send_udp_packet(0, NULL);

        // sleep for 10s
        ztimer_sleep(ZTIMER_MSEC, 10*1000);
    }

    /* should be never reached */
    return 0;
}

/*
 * peer.c
 *
 * Authors: Ed Bardsley <ebardsle+441@andrew.cmu.edu>,
 *          Dave Andersen
 * Class: 15-441 (Spring 2005)
 *
 * Skeleton for 15-441 Project 2.
 *
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <time.h>

#include "config.h"
#include "lib/dplist.h"
#include "lib/mtcp.h"
#include "lib/bt_parse.h"
#include "lib/debug.h"
#include "lib/input_buffer.h"
#include "lib/spiffy.h"

// typedef struct packet_info
// {
//     time_t ts;      // the time when packet is send-> timeout packet will be retransmitted
//     seqnr_t seqnr;      // sequence number of the packet
//     // char acks;         // number of ACKs received for this packet, 0: unacked
// }packet_info_t;


typedef struct peer_node
{
    int id;     // peer id in nodes.map
    u_int32_t port;  // peer port number in nodes.map
    char in_trans;      // 1: transmitting data with this node FIXME: other status?
    mtcp_conn_t* conn;   // a TCP connection, NULL if no data is transmitting
}peer_node_t;



// golabl variables for this file
dplist_t * peer_list = NULL;        // a list of all peers


// callback functions for peer_list
void *peer_node_copy(void *src_element); 
void peer_node_free(void **element);
int peer_node_comp(void *x, void *y);


void peer_run(bt_config_t *config);

int main(int argc, char **argv)
{
    bt_config_t config;

    bt_init(&config, argc, argv); // configure map, chunks, file location etc.

    // my own variables init
    peer_list = dpl_create(&peer_node_copy, &peer_node_free, &peer_node_comp);

    DPRINTF(DEBUG_INIT, "peer.c main beginning\n");

#ifdef TESTING
    config.identity = 1; // your group number here
    strcpy(config.chunk_file, "chunkfile");
    strcpy(config.has_chunk_file, "haschunks");
#endif

    bt_parse_command_line(&config);

#ifdef DEBUG
    if (debug & DEBUG_INIT)
    {
        bt_dump_config(&config);
    }
#endif

    peer_run(&config); // run the peer (accept user input)
    return 0;
}

/**
 * core logic of TCP based on UDP: the reliability, congestion control logic goes here
 */
void process_inbound_udp(int sock)
{
#define BUFLEN 1500
    struct sockaddr_in from;       // these information will be used to process the packet: peer_info
    socklen_t fromlen;
    char buf[BUFLEN];

    fromlen = sizeof(from);
    spiffy_recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *)&from, &fromlen);

    data_packet_t * udp_packet = (data_packet_t *)buf;      // analyze the data
    // TODO: write code to process received UDP packet: seq_num, ack_num etc.
    printf("PROCESS_INBOUND_UDP SKELETON -- replace!\n"
           "Incoming message from %s:%d\n%s\n\n",
           inet_ntoa(from.sin_addr), ntohs(from.sin_port), buf);

    switch (udp_packet->header.packet_type)
    {
    case 0:
        // WHOHAS
        break;
    case 1:
        //IHAVE
        break;
    case 2:
        // GET request
        break;
    case 3:
        // DATA

        break;
    case 4: 
        // ACK
        
        break;
    case 5:
        // DENIED
        break;
    default:
        printf("Not defined packet type, posibbly due to bit error.\n");
        break;
    }
}

/**
 * function to handle GET command, to be finished
 */
void process_get(char *chunkfile, char *outputfile)
{
    //TODO: finish the logic behind GET query--process hash and compare it with maps
    printf("PROCESS GET SKELETON CODE CALLED.  Fill me in!  (%s, %s)\n",
           chunkfile, outputfile);
}

/** function to handle user command input, classify it into different type and run
 */
void handle_user_input(char *line, void *cbdata)
{
    char chunkf[128], outf[128];

    bzero(chunkf, sizeof(chunkf));
    bzero(outf, sizeof(outf));

    //TODO: add code for other command (extra function)
    if (sscanf(line, "GET %120s %120s", chunkf, outf))
    {
        if (strlen(outf) > 0)
        {
            process_get(chunkf, outf);
        }
    }
}

/** core function of the program
 * config: map, chunks, and other information needed to run the peer
 */
void peer_run(bt_config_t *config)
{
    int sock;
    struct sockaddr_in myaddr;
    fd_set readfds;
    struct user_iobuf *userbuf;

    if ((userbuf = create_userbuf()) == NULL)
    {
        perror("peer_run could not allocate userbuf");
        exit(-1);
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1)
    {
        perror("peer_run could not create socket");
        exit(-1);
    }

    bzero(&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(config->myport);

    if (bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1)
    {
        perror("peer_run could not bind socket");
        exit(-1);
    }

    spiffy_init(config->identity, (struct sockaddr *)&myaddr, sizeof(myaddr));

    while (1)
    {
        int nfds;
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        nfds = select(sock + 1, &readfds, NULL, NULL, NULL);

        if (nfds > 0)
        {
            if (FD_ISSET(sock, &readfds))
            {
                process_inbound_udp(sock);  // data from socket: IP:port, stateless
            }

            if (FD_ISSET(STDIN_FILENO, &readfds))
            {
                // user command input
                process_user_input(STDIN_FILENO, userbuf, handle_user_input,
                                   "Currently unused");
            }
        }
    }
}


// callback functions for peer_list
void *peer_node_copy(void *src_element)
{
    peer_node_t *tmp = (peer_node_t*) src_element;
    peer_node_t * new_node = malloc(sizeof(peer_node_t));
    new_node->id = tmp->id;
    new_node->port = tmp->port;
    new_node->in_trans = tmp->in_trans;
    
    new_node->conn = mtcp_conn_copy(tmp->conn);
    return new_node;

}
void peer_node_free(void **element)
{
    peer_node_t *tmp = (peer_node_t*) (*element);
    mtcp_conn_free((void**)&tmp->conn);
    free(tmp);
}
int peer_node_comp(void *x, void *y)
{
    peer_node_t* x1 = (peer_node_t*)x;
    peer_node_t* y1 = (peer_node_t*)y;

    if(x1->id<y1->id){
        return -1;
    }
    else if(x1->id>y1->id){
        return 1;
    }
    return 0;
}

/* my implementation of TCP */

#ifndef _MTCP_H_
#define _MTCP_H_

#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#include "packet.h"
#include "dplist.h"

// MTCP configs
#define LIST_END 9        // the end index of any lists
#define WINDOW_SIZE 8       // MTCP sliding window size

// function return status code

// return status for mtcp_process_data
#define MTCP_OLD_DATA    29   // an old data packet was received (out of order)
#define MTCP_INORDER_NEW   30 // a new in order data packet 
#define MTCP_OUTORDER_NEW  31 // a new out of order packet

// return status for mtcp_process_ack
#define MTCP_SENT_SUCCESS  0
#define MTCP_RESEND  41
// #define MTCP_INORDER_ACK 40
#define MTCP_SEND_WAIT 42   // send list buffer is full, wait
#define MTCP_SEND_CONT 40   // tell MBT to continue sending

#define TRUE 1
#define FALSE 0

#define MTCP_ERROR -1;

// the struct used in to trace time
typedef struct buf_packet
{
    data_packet_t packet;
    time_t ts;  // the timestamp when the packet was sent
} buf_packet_t; //

typedef struct mtcp_conn
{
    // information to maintain a TCP connection
    struct sockaddr_in socket;  //IP:port
    
    // for receiver
    acknr_t next_expect;    // expected sequence number of next packet, ack_num in receiver ACK packet, cumulative ack
    dplist_t *recv_packets; // received packets

    // for sender:
    dplist_t *sent_packets; // timestamps used for retransmission, data type: buf_packet_t, include time info
    acknr_t last_ack;       // last acked packet, for out of order packets
    char acks;              // number of acks received: three duplicate ACKs will trigger fast retransmission
    seqnr_t send_base;      // sequence number of the oldest unacknowledged packet
    seqnr_t next_send;      // sequence number of next packet
    u_int32_t rtdelay;      // round trip delay in us TODO: implement sampleRTT to estimate RTT

} mtcp_conn_t;

// callback functions used for buf_packet list: sent packets
void *buf_packet_copy(void *src_element);
void buf_packet_free(void **element);
int buf_packet_comp(void *x, void *y);

// callback functions for mtco_conn list
void *mtcp_conn_copy(void *src_element);
void mtcp_conn_free(void **element);
int mtcp_conn_comp(void *x, void *y);

// core functions for reliable data transfer


/* sender side functions */
/**
 * send a data packet and update information in the connection: seqnum, ack num etc.
 * as well as the sent_packets list
 */
int mtcp_send_packet(mtcp_conn_t* conn, data_packet_t* data_packet, char isnew);

/**
 * process received ACK packet, store into recv_packets list, update ack number
 * 
 * retransmission handled here? yes, retransmission triggered by 3 duplicate ACKs is started here
 * time out retransmission is in main loop
 * 
 * new data transmission is initialized by the higher level process
 */
int mtcp_process_ack(mtcp_conn_t* conn, ack_packet_t* ack_packet);

/* sender side functions end*/


/* receiver side functions */
/**
 * process received data packet, update rev_packets list, send ACK back
 */
int mtcp_process_data(mtcp_conn_t* conn, data_packet_t* data_packet);

/**
 * send an ACK packet, this function should only be called within mtcp.h
 */
int mtcp_send_ack(mtcp_conn_t* conn, ack_packet_t* ack_packet);
/* receiver side functions end */


#endif
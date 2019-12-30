/* my implementation of TCP */

#ifndef _MTCP_H_
#define _MTCP_H_

#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

typedef u_int seqnr_t;    // sequence number type
typedef u_int acknr_t;  // ACK number type

typedef struct header_s {
  short magicnum;
  char version;
  char packet_type;
  short header_len;
  short packet_len; 
  u_int seq_num;
  u_int ack_num;
} header_t;  


#define BUFLEN 100
#define DATALEN 13  // maximum data length

typedef struct data_packet
{
    header_t header;
    // char data[BUFLEN];
    char *data;
    // time_t ts;    // the timestamp when the packet was sent
} data_packet_t;


// the struct used in to trace time
typedef struct buf_packet
{
  data_packet_t packet;
  time_t ts;    // the timestamp when the packet was sent
}buf_packet_t;    // 


#define TYPE_LEN 10  // max length of the type string

/** return a string indicating the type of an integer
 */
char * decode_packet_type(char code);

// callback functions used for data_packet list: received packets
void *packet_copy(void *src_element); 
void packet_free(void **element);
int packet_comp(void *x, void *y);

// callback functions used for buf_packet list: sent packets
void *buf_packet_copy(void *src_element); 
void buf_packet_free(void **element);
int buf_packet_comp(void *x, void *y);


#endif
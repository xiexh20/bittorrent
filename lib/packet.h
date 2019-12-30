#ifndef _PACKET_H
#define _PACKET_H

#include <sys/types.h>

//datatype of mtcp packet
typedef uint32_t seqnr_t;    // sequence number type
typedef uint32_t acknr_t;  // ACK number type

#define BUFLEN 100
#define DATALEN 1300  // maximum data length
#define TYPE_LEN 10  // max length of the type string

typedef struct header_s {
  short magicnum;
  char version;
  char packet_type;
  short header_len;
  short packet_len; 
  seqnr_t seq_num;
  acknr_t ack_num;
} header_t;  

typedef struct data_packet
{
    header_t header;
    // char data[BUFLEN];
    char data[DATALEN];
    // time_t ts;    // the timestamp when the packet was sent
} data_packet_t;


/** return a string indicating the type of an integer
 */
char * decode_packet_type(char code);

// callback functions used for data_packet list: received packets
void *packet_copy(void *src_element); 
void packet_free(void **element);
int packet_comp(void *x, void *y);

#endif      /* _PACKET_H_ */
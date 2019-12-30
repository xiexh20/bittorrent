#ifndef _PACKET_H
#define _PACKET_H

#include <sys/types.h>
#include <inttypes.h>

//datatype of mtcp packet
typedef uint32_t seqnr_t;    // sequence number type
typedef uint32_t acknr_t;  // ACK number type

#define BUFLEN 100
#define DATALEN 1000  // maximum data length
#define ACK_DATALEN 10  // payload of ack packet
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
    uint8_t data[DATALEN];
    // time_t ts;    // the timestamp when the packet was sent
} data_packet_t;

typedef struct ack_packet
{
    header_t header;
    char data[ACK_DATALEN];     // use a different size of data compared to data packet, to save memory space
}ack_packet_t;


/** return a string indicating the type of an integer
 */
char * decode_packet_type(char code);

// callback functions used for data_packet list: received packets
void *packet_copy(void *src_element); 
void packet_free(void **element);
int packet_comp(void *x, void *y);

#endif      /* _PACKET_H_ */
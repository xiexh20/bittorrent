/* my implementation of TCP */

#ifndef _MTCP_H_
#define _MTCP_H_

#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct header_s {
  short magicnum;
  char version;
  char packet_type;
  short header_len;
  short packet_len; 
  u_int seq_num;
  u_int ack_num;
} header_t;  


typedef struct data_packet
{
    header_t header;
    char data[BUFLEN];
} data_packet_t;


#define TYPE_LEN 10  // max length of the type string

/** return a string indicating the type of an integer
 */
char * decode_packet_type(char code);

#endif
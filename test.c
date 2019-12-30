/** a simple test code */
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

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
    time_t ts;    // the timestamp when the packet was sent
} data_packet_t;

void *packet_copy(void *src_element);
void print_header(header_t header);

int main()
{
    int i = 0;
    for(i=0;i<5;i++)
    {
        header_t header;
        header.seq_num = i;
        header.version = 'A';
        header.packet_type = i;
        header.ack_num = i+20;
        data_packet_t* packet = malloc(sizeof(data_packet_t));
        packet->header = header;
        packet->data = malloc(DATALEN*sizeof(char));
        memcpy(packet->data, "abcdefg", DATALEN);
        printf("In main: %s\n", packet->data);

        data_packet_t* copied = packet_copy(packet);
        print_header(copied->header);
        
        free(packet->data);
        free(copied->data);
        free(copied);
        free(packet);
        // free(header);
    }
    return 0;

}

// /** print out information of received header
//  */
// void print_header(header_t header)
// {
//     printf("MAGIC: %d\n", ntohs((header).magicnum));
//     printf("Version: %c\n", header.version);
    
//     // char *type = decode_packet_type(header.packet_type);
//     printf("packet_type: %d\n", header.packet_type);
//     // free(type);

//     printf("header length:%d\n", ntohs(header.header_len));
//     printf("packet length: %d\n", ntohs(header.packet_len));
//     printf("sequence number: %d\n", ntohs(header.seq_num));
//     printf("ACK number: %d\n", ntohs(header.ack_num));
//     fflush(stdout);
// }

// /** deep copy a packet
//  */
// void *packet_copy(void *src_element)
// {
//     data_packet_t * tmp = (data_packet_t*) src_element;
//     data_packet_t *new_packet = malloc(sizeof(data_packet_t));
//     new_packet->ts = tmp->ts;       // long type, copy directly
//     new_packet->header = tmp->header;       // TODO: is this a deep copy?

//     new_packet->data = malloc(DATALEN*sizeof(char));
//     printf("Src str:%s\n", tmp->data);
//     memcpy(new_packet->data, tmp->data, DATALEN);   
//     printf("Copied str:%s\n", new_packet->data);
//     // FIXME: copy data correctly
//     return new_packet;
// }

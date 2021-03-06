#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "packet.h"
/** return a string indicating the type of an integer
 */
char * decode_packet_type(char code)
{
    char* type = malloc(TYPE_LEN*sizeof(char));
    switch (code)
    {
    case 0:
        memcpy(type, "WHOHAS", TYPE_LEN);
        break;
    case 1:
        memcpy(type, "IHAVE", TYPE_LEN);
        break;
    case 2:
        memcpy(type, "GET", TYPE_LEN);
        break;
    case 3:
        memcpy(type, "DATA", TYPE_LEN);
        break;
    case 4:
        memcpy(type, "ACK", TYPE_LEN);
        break;
    case 5:
        memcpy(type, "DENIED", TYPE_LEN);
        break;
    default:
        memcpy(type, "NOTDEFINED", TYPE_LEN);
        break;
    }
    return type;

}

// callback functions used for packet list
/** deep copy a packet
 */
void *packet_copy(void *src_element)
{
    data_packet_t * tmp = (data_packet_t*) src_element;
    data_packet_t *new_packet = malloc(sizeof(data_packet_t));
    // new_packet->ts = tmp->ts;       // long type, copy directly
    new_packet->header = tmp->header;       // TODO: is this a deep copy?

    // new_packet->data = malloc(DATALEN*sizeof(char));
    printf("Src str:%s\n", tmp->data);
    memcpy(new_packet->data, tmp->data, DATALEN); 
    printf("Copied str:%s\n", new_packet->data);  
    // FIXME: copy data correctly
    return new_packet;
}
/** free memory of a packet
 */
void packet_free(void **element)
{
    data_packet_t * tmp = (data_packet_t*) (*element);
    // free(tmp->data);
    free(tmp);
}
/** 
 * compare two packet based on sequence number
 */
int packet_comp(void *x, void *y)
{
    data_packet_t * x1 = (data_packet_t*) x;
    data_packet_t * y1 = (data_packet_t*) y;

    if(x1->header.seq_num<y1->header.seq_num){
        return -1;
    }
    else if(x1->header.seq_num > y1->header.seq_num){
        return 1;
    }
    return 0;
}
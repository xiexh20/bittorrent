#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mtcp.h"


void *buf_packet_copy(void *src_element)
{
    buf_packet_t *tmp = (buf_packet_t*) src_element;
    buf_packet_t *new_buf = malloc(sizeof(buf_packet_t));
    new_buf->packet.header = tmp->packet.header;
    // new_buf->packet.data = malloc(DATALEN*sizeof(char));
    memcpy(new_buf->packet.data, tmp->packet.data, DATALEN);
    new_buf->ts = tmp->ts;
    return new_buf;
}
void buf_packet_free(void **element)
{
    buf_packet_t* tmp = (buf_packet_t*)(*element);
    // free(tmp->packet.data);
    free(tmp);
}
int buf_packet_comp(void *x, void *y)
{
    buf_packet_t *x1 = (buf_packet_t*) x;
    buf_packet_t *y1 = (buf_packet_t*) y;
    return packet_comp(&x1->packet, &y1->packet);
}


// callback functions
void *mtcp_conn_copy(void *src_element)
{
    mtcp_conn_t *tmp = (mtcp_conn_t*) src_element;
    mtcp_conn_t * new_conn = malloc(sizeof(mtcp_conn_t));

    new_conn->sent_packets = tmp->sent_packets;
    new_conn->next_send = tmp->next_send;
    new_conn->last_ack = tmp->last_ack;
    new_conn->acks = tmp->acks;
    new_conn->next_expect = tmp->next_expect;
    new_conn->sent_packets = tmp->sent_packets; // not deep copy
    new_conn->recv_packets = tmp->recv_packets;

    return new_conn;
}
void mtcp_conn_free(void **element)
{
    mtcp_conn_t * tmp = (mtcp_conn_t*) (*element);
    dpl_free(&tmp->recv_packets, 1);
    dpl_free(&tmp->sent_packets, 1);
    free(tmp);
}

/**
 * sorted by RTT
 */
int mtcp_conn_comp(void *x, void *y)
{
    mtcp_conn_t* x1 = (mtcp_conn_t*)x;
    mtcp_conn_t* y1 = (mtcp_conn_t*)y;

    if(x1->rtdelay<y1->rtdelay){
        return -1;
    }
    else if(x1->rtdelay>y1->rtdelay){
        return 1;
    }
    return 0;

}
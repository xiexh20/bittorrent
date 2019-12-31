#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mtcp.h"
#include "debug.h"


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

/* sender side functions */
/**
 * send a data packet and update information in the connection: seqnum, ack num etc.
 * as well as the sent_packets list
 * seq and ack num logic is not handled here
 * this packet only do insertion and send
 */
int mtcp_send_packet(mtcp_conn_t* conn, data_packet_t* data_packet, char isnew)
{
    // add the data packet to the connection send list
    if(isnew==TRUE){
        dpl_insert_at_index(conn->sent_packets, data_packet, LIST_END, 0);  // insert the packet into the end of the list
    }
    
    return MTCP_SENT_SUCCESS;
}

/**
 * process received ACK packet, store into recv_packets list, update ack number
 * 
 * retransmission handled here? yes, retransmission triggered by 3 duplicate ACKs is started here
 * time out retransmission is in main loop
 * 
 * new data transmission is initialized by the higher level process
 */
int mtcp_process_ack(mtcp_conn_t* conn, ack_packet_t* ack_packet)
{
    // convert network number into host number
    acknr_t ack = ntohl(ack_packet->header.ack_num);
    if(ack==conn->last_ack){
        // duplicate ack
        conn->acks++;
        if(conn->acks==3){
            // retransmit the oldest packet
            data_packet_t *old = dpl_get_element_at_index(conn->sent_packets, 0);   // get the oldest packet
            mtcp_send_packet(conn, old, FALSE); // send an old packet, do not add it into the list
            return MTCP_RESEND;
        }
        else{
            // check if the list buffer is full
            if(dpl_size(conn->sent_packets)==WINDOW_SIZE){
                return MTCP_SEND_WAIT;  // wait 
            }

            // still some space available for the buffer list
            return MTCP_SEND_CONT;
        }
    }
    else if(ack==conn->last_ack+1){
        // an inoder ack packet, remove the first data packet buffer
        dpl_remove_at_index(conn->sent_packets, 0, FALSE);  // do not free element at this moment
        conn->last_ack = ack;
        conn->acks = 1;
        conn->send_base++;  // move window forward
        conn->next_send++;  // FIXME: is this correct?
        return MTCP_SEND_CONT;    // tell the MBT to load new data into the buffer
    }
    else if(ack>conn->last_ack+1)
    {
        // several packets are acked
        //TODO: remove all packets that have seqnr<=ack
        //update information in the conn_t
        return MTCP_SEND_CONT; // tell the MBT to load new data
    }
    else{
        // should not be possible since the receiver only send next expect seq
        DPRINTF("An impossible case in mtcp_process_ack, debug it!\n");
        return MTCP_ERROR;
    }
}

/* sender side functions end*/
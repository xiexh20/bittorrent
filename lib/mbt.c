#include <stdlib.h>

#include "mbt.h"


/**
 * deep copy a mbt_buf
 */
void *mbt_buf_copy(void *src_element)
{
    mbt_buf_t * tmp = (mbt_buf_t*) src_element;
    mbt_buf_t * new_buf = malloc(sizeof(mbt_buf_t));
    new_buf->conn = mtcp_conn_copy(tmp->conn);
    new_buf->peer_id = tmp->peer_id;
    memcpy(new_buf->hashcode, tmp->hashcode, HASH_LEN);
    new_buf->buffer = tmp->buffer;     // TODO: make this a deep copy
    new_buf->dbase = tmp->dbase;
    new_buf->dnext = tmp->dnext;
    new_buf->reqt = tmp->reqt;
    return new_buf;
}
void mbt_buf_free(void **element)
{
    mbt_buf_t * tmp = (mbt_buf_t*)(*element);
    mtcp_conn_free((void**)&tmp->conn);
    // TODO: free buffer
    free(tmp);
}

/** compare two buffer based on peer id
 */
int mbt_buf_comp(void *x, void *y)
{
    mbt_buf_t *x1 = (mbt_buf_t*)x;
    mbt_buf_t *y1 = (mbt_buf_t*)y;

    if(x1->peer_id<y1->peer_id){
        return -1;
    }
    else if(x1->peer_id>y1->peer_id){
        return 1;
    }
    return 0;
}

/**
 * construct a temprary mbt_buf variable and call dplist functions to find buf
 */
mbt_buf_t * find_buf_by_id(dplist_t * buf_list, short id)
{
    mbt_buf_t * tmp = malloc(sizeof(mbt_buf_t));
    tmp->peer_id = id;
    int index = dpl_get_index_of_element(buf_list, tmp);
    free(tmp);
    return dpl_get_element_at_index(buf_list, index);
}


/** sender side functions */
/**
 * process received whohas packet
 * extract chunk hashcode, compare them with the hash code in .haschunks file
 * send IHAVE packet out or NULL
 * required input: 
 * a WHOHAS packet
 * .haschunks file
 * information of the peer who sent the WHOHAS packet
 * return: a status code
 * 
 * params: 
 * config: a struct contains all file information
 * peer: host info(ip addr, port) who sent the WHOHAS packet
 */
int mbt_process_whohas(bt_config_t * config, bt_peer_t * peer, data_packet_t * whohas_packet)
{
    // get number of hashcodes in the packet
    int num = 0, ihave_count=0;        //TODO: a function to read number of chunk hash codes from the packet
    int i = 0;
    hash_type *ihave[num];  // an array to store hash code I have
    for(i=0;i<num;i++){
        hash_type hash[HASH_LEN];   // TODO: a function to extract hash code from the packet
        // TODO: a function to compare two hash code
        int result = 0; // compare result
        if(result==HASH_EQUAL){
            ihave[ihave_count] = (hash_type*)malloc(HASH_LEN*sizeof(hash_type));
            memcpy(ihave[ihave_count], hash, HASH_LEN);
            ihave_count++;
        }
    }
    if(ihave_count>0){
        // TODO: send a IHAVE packet
        // initialize a IHAVE packet, using the ihave hash code array
        return MBT_IHAVE_SENT;
    }
    return MBT_NO_CHUNK;
}

/** process received GET packet
 * parse the chunks file and initialize a MTCP transmission: mtcp_conn, etc...
 * load data to the chunk buffer(chunk_request)
 * required input:
 * a GET packet
 * path to the chunk
 * information of the peer who sent the GET packet
 * modified: a mtcp connection
 * after this, the peer is in transmission status with another peer
 * 
 * params: 
 * send_buf: an empty chunk of data to be filled with the filepath provied by config, a mtcp_conn to be initialized
 */
int mbt_process_get(bt_config_t* config, bt_peer_t* peer,
            mbt_buf_t * send_buf, data_packet_t *get_packet)
{
    //TODO: extract hashcode, and convert hashcode to a chunk file path
    send_buf = malloc(sizeof(mbt_buf_t));

    // init a  MTCP connection
    send_buf->conn = malloc(sizeof(mtcp_conn_t));

    // init a data packet and send it
    data_packet_t data_packet;

    // update the buffer

    return MBT_DATA_SENT;
}


/** process received ACK packet
 * call mtcp_process_ack to process the packet
 * if all data transmitted, stop 
 * still some data to be transmitted, continue transmission
 * required input:
 * a .chunks file or a buffer
 * mtcp_conn
 * chunks sent history (what has been sent?) a pointer? 
 * 
 * return: status code
 * in transmission, sent done, error
 */
int mbt_process_ack(mbt_buf_t* send_buf, ack_packet_t * ack_packet)
{
    mtcp_process_ack(send_buf->conn, ack_packet);       // update sequence info of the mtcp_conn

    // check status of the mtcp_conn, update the buffer pointers
    // three options: 
    // transmit new data; 
    // retransmit the oldest data packet (3 duplicate ACKs)
    // or terminate transmission (bad connection or all data transmitted)
    return MBT_DATA_SENT;
}

/** sender side functions done */


/** receiver side (a downloader) functions */

/** process received IHAVE packet
 * send a GET to the host, initialize a receiver buffer (chunk_buf)
 * required input:
 * a IHAVE packet
 * information of the peer who sent the IHAVE packet
 * how to implement a timer for resending GET?
 * return: a status code
 * param: 
 * peer: host info(ip addr, port) of the peer who sent IHAVE packet
 * recv_buf: the receiver buffer, to be initialized
 */
int mbt_process_ihave(bt_peer_t * peer, mbt_buf_t* recv_buf, data_packet_t * ihave_packet)
{
    //TODO: initialize a recv_buf: fill mtcp connection in
    recv_buf = malloc(sizeof(mbt_buf_t));
    recv_buf->peer_id = peer->id;
    recv_buf->conn = malloc(sizeof(mtcp_conn_t));
    // TODO: a function to init a mtcp_conn
    recv_buf->conn->acks = 0;
    recv_buf->reqt = time(NULL);    // get current time

    //TODO: initialize a GET packet using the IHAVE packet, and send the packet
    data_packet_t get_packet;
    //send a GET packet

    return MBT_GET_SENT;
}

/** process received DATA packet
 * call mtcp_process_data to process the packet
 * check if this chunk has been downloaded completely ==>> compare it with the hashcode of the GET packet
 * required input: 
 * information of the sender
 * a mtcp_conn
 * a buffer of all downloaded data
 * hashcode of the chunk (from the GET packet that initialize the transmission) 
 * -->> a list of send_buf? or a struct of chunk_send_buf, contains mtcp_conn?
 * return: status code--sucess, done, or error
 * 
 * params:
 * recv_buf: a send_buf chunk contains received data and information of the mtcp_conn
 */
int mbt_process_data(mbt_buf_t* recv_buf, data_packet_t * data_packet)
{
    int status = mtcp_process_data(recv_buf->conn, data_packet);        // the ACK was sent out inside this function
    if(status==MTCP_INORDER_NEW){
        // move base pointer (for the recv_buf) forward
        // load the in order packet data to the buffer
        // compute hash code to check if data is finished
        return MBT_RECV_DONE;
    }
    else if(status==MTCP_OUTORDER_NEW){
        // do not move the base pointer
        // do not compute and compare hash code
        return MBT_RECV_CONT;

    }
    else if(status==MTCP_OLD_DATA){
        // discard it??
        return MBT_RECV_CONT;
    }
    return MBT_RECV_CONT;
}

/** receiver side functions end */
#ifndef _MBT_H
#define _MBT_H
/* BitTorrent functionalities */


#include "packet.h"
#include "mtcp.h"
#include "sha.h"
#include "bt_parse.h"

#define HASH_LEN SHA1_HASH_SIZE         // the length of a chunk hashcode


/* data struct for receiver and sender */
typedef struct mbt_buf
{
    mtcp_conn_t * conn;
    short peer_id;          // to identify the request, each peer can only have one request
    uint8_t hashcode[HASH_LEN];    // the hashcode of the request chunk, set when sending GET packet
    uint8_t *buffer;        // buffer for the chunk, possibly a 512*1024 char
    uint8_t *dnext;            // data pointer, the position of next block of data to be sent/the position of next received data
    uint8_t *dbase;          // for sender, the base pointer
    time_t reqt;            // the time when sender receive the GET request or last time the receiver send the GTE request
                            // this will be checked by the peer. receiver side: if time out, send GET packet again
}mbt_buf_t;


// callback functions
void *mbt_buf_copy(void *src_element); 
void mbt_buf_free(void **element);
int mbt_buf_comp(void *x, void *y);

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
int mbt_process_whohas(bt_config_t * config, bt_peer_t * peer, data_packet_t * whohas_packet);


/** process received IHAVE packet
 * send a GET to the host, initialize a receiver buffer (chunk_request)
 * required input:
 * a IHAVE packet
 * information of the peer who sent the IHAVE packet
 * how to implement a timer for resending GET?
 * return: a status code
 * param: 
 * peer: host info(ip addr, port) of the peer who sent IHAVE packet
 * request: the receiver buffer, to be initialized
 */
int mbt_process_ihave(bt_peer_t * peer, mbt_buf_t* request, data_packet_t * ihave_packet);

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
 * request: an empty chunk of data to be filled with the filepath provied by config, a mtcp_conn to be initialized
 */
int mbt_process_get(bt_config_t* config, bt_peer_t* peer,
            mbt_buf_t * request, data_packet_t *get_packet);


/** process received DATA packet
 * call mtcp_process_data to process the packet
 * check if this chunk has been downloaded completely ==>> compare it with the hashcode of the GET packet
 * required input: 
 * information of the sender
 * a mtcp_conn
 * a buffer of all downloaded data
 * hashcode of the chunk (from the GET packet that initialize the transmission) 
 * -->> a list of request? or a struct of chunk_request, contains mtcp_conn?
 * return: status code--sucess, done, or error
 * 
 * params:
 * request: a request chunk contains received data and information of the mtcp_conn
 */
int mbt_process_data(mbt_buf_t* request, data_packet_t * data_packet);

/** process received ACK packet
 * call mtcp_process_ack to process the packet
 * if all data transmitted, stop 
 * still some data to be transmitted, continue transmission
 * required input:
 * a .chunks file
 * mtcp_conn
 * chunks sent history (what has been sent?) a pointer? 
 * 
 * return: status code
 * in transmission, sent done, error
 */
int mbt_process_ack(mbt_buf_t* request, ack_packet_t * ack_packet);

#endif
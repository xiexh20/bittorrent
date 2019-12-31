#ifndef _MBT_H
#define _MBT_H
/* BitTorrent functionalities */


#include "packet.h"
#include "mtcp.h"
#include "sha.h"
#include "bt_parse.h"

#define HASH_LEN SHA1_HASH_SIZE         // the length of a chunk hashcode
#define HASH_EQUAL 0
#define HASH_NOT_EQUAL 1

// function return status code
#define MBT_SUCCESS 0       // successfully processed the packet
#define MBT_IHAVE_SENT 1    // a IHAVE packet was sent out
#define MBT_GET_SENT 2      // a GET packet was sent out
#define MBT_DATA_SENT 3     // new data sent out, different from resend
#define MBT_ACK_SENT 4
#define MBT_DENIED_SENT 5   // a DENIED packet was sent out
#define MBT_NO_CHUNK 11     // in response to the WHOHAS packet

#define MBT_SEND_DONE  30  // this chunk has been transmitted successfully
#define MBT_RESEND  31      // resent a data packet
#define MBT_SEND_FAIL 32   // an ongoing transmission was terminated by some error

#define MBT_RECV_DONE 40    // a chunk has been downloaded successfully
#define MBT_RECV_CONT 41    // continuing downloading

typedef uint8_t hash_type;

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

// assistant functions
/**
 * construct a temprary mbt_buf variable and call dplist functions to find buf
 */
mbt_buf_t * find_buf_by_id(dplist_t * buf_list, short id);

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
 * recv_buf: the receiver buffer, to be initialized
 */
int mbt_process_ihave(bt_peer_t * peer, mbt_buf_t* recv_buf, data_packet_t * ihave_packet);

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
            mbt_buf_t * send_buf, data_packet_t *get_packet);


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
int mbt_process_data(mbt_buf_t* recv_buf, data_packet_t * data_packet);

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
int mbt_process_ack(mbt_buf_t* send_buf, ack_packet_t * ack_packet);

#endif
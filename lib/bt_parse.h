/*
 * bt_parse.h
 *
 * Initial Author: Ed Bardsley <ebardsle+441@andrew.cmu.edu>
 * Class: 15-441 (Spring 2005)
 *
 * Skeleton for 15-441 Project 2 command line and config file parsing
 * stubs.
 *
 */

#ifndef _BT_PARSE_H_
#define _BT_PARSE_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BT_FILENAME_LEN 255
#define BT_MAX_PEERS 1024

typedef struct bt_peer_s {
  short  id;
  struct sockaddr_in addr;
  struct bt_peer_s *next;
} bt_peer_t;

struct bt_config_s {
  char  chunk_file[BT_FILENAME_LEN];  //.chunk, real data
  char  has_chunk_file[BT_FILENAME_LEN];  //.haschunk file path, contain info of chunks for this peer
  char  output_file[BT_FILENAME_LEN];   // output path for received chunks
  char  peer_list_file[BT_FILENAME_LEN];  // nodes.map filepath, topology of the BitTorrent
  int   max_conn;   // maximum number of TCP connection? 
  short identity; // id of the peer
  unsigned short myport;

  int argc; 
  char **argv;

  bt_peer_t *peers;   // a list of peers in the BitTorrent network
};
typedef struct bt_config_s bt_config_t;


void bt_init(bt_config_t *c, int argc, char **argv);
void bt_parse_command_line(bt_config_t *c);
void bt_parse_peer_list(bt_config_t *c);
void bt_dump_config(bt_config_t *c);
bt_peer_t *bt_peer_info(const bt_config_t *c, int peer_id);

#endif /* _BT_PARSE_H_ */
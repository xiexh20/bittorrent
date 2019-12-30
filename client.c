#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "lib/mtcp.h"
#include <stdlib.h>

#include "lib/spiffy.h"

#define PACKETLEN 1500
#define BUFLEN 100

// typedef struct header_s {
//   short magicnum;
//   char version;
//   char packet_type;
//   short header_len;
//   short packet_len; 
//   u_int seq_num;
//   u_int ack_num;
// } header_t;  

// typedef struct data_packet {
//   header_t header;
//   char data[BUFLEN];
// } data_packet_t;


int main(int argc, char **argv) {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in myaddr, toaddr;
  socklen_t fromlen;
  char buf[BUFLEN];

  data_packet_t packet;

  assert(fd != 0);
  if (argc != 6) {
    // printf("usage: %s <node id> <my port> <to port> <magic number>\n", argv[0]);
    printf("usage: %s <magic number> <seq num> <ack num> <type> <data str>\n", argv[0]);
    return 1;
  }

  
  
  packet.header.magicnum = htons(atoi(argv[1]));
  memset(&(packet.data), 0, BUFLEN);      // FIXME: why this will clear toaddr???

  // for debug
  packet.header.version = 'A';
  packet.header.packet_type = 'B';
  packet.header.seq_num = htonl(atoi(argv[2]));
  packet.header.ack_num = htonl(atoi(argv[3]));
  packet.header.header_len = sizeof(packet.header);
  packet.header.packet_type = *argv[4] - '0';   // read a char
  // packet.data = malloc(10*sizeof(char));
  memcpy(packet.data, argv[5], 10);

  // send init
  bzero(&myaddr, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  inet_aton("127.0.0.1", &myaddr.sin_addr);
  // myaddr.sin_port = htons(atoi(argv[2]));
  myaddr.sin_port = htons(atoi("48002"));
  assert(bind(fd, (struct sockaddr *) &myaddr, sizeof(myaddr)) >= 0);

  /* init spiffy */
  // spiffy_init(atoi(argv[1]), &myaddr, sizeof(myaddr));
  spiffy_init(atoi("2"), &myaddr, sizeof(myaddr));

  inet_aton("127.0.0.1", &toaddr.sin_addr);
  // toaddr.sin_port = htons(atoi(argv[3]));
  toaddr.sin_port = htons(atoi("48001"));
  toaddr.sin_family = AF_INET;

  spiffy_sendto(fd, &packet, sizeof(data_packet_t), 0, (struct sockaddr *) &toaddr, sizeof(toaddr));

  // printf("Header sent: %d\n", atoi(argv[4]));
  printf("Host %s:%d sent packet:\n", inet_ntoa(myaddr.sin_addr), ntohs(myaddr.sin_port));
  printf("Header:\n");
  print_header(packet.header);
  printf("data: %s\n", packet.data);

  return 0;
}

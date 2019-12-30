/* test my TCP implementation */
#include <stdlib.h>

#include "lib/mtcp.h"
#include "lib/dplist.h"
#include "lib/debug.h"

int main()
{
    // dplist_t * packets = NULL;
    // packets = dpl_create(&packet_copy, &packet_free, packet_comp);

    int i = 0;
    // for(i=0;i<5;i++)
    // {
    //     header_t header;
    //     header.seq_num = i;
    //     header.version = 'A';
    //     header.packet_type = i;
    //     header.ack_num = i+20;
    //     header.magicnum = i * 3;
    //     header.packet_len = 100;
        
    //     data_packet_t *packet = malloc(sizeof(data_packet_t));
    //     packet->header = header;
    //     packet->data = malloc(DATALEN*sizeof(char));
    //     memcpy(packet->data, "abcdefg", DATALEN);

    //     dpl_insert_at_index(packets, packet, 0, 1);
    //     data_packet_t * tmp = dpl_get_element_at_index(packets, 0);
    //     print_header(tmp->header);
    //     free(packet->data);
    //     free(packet);
    // }

    // dpl_free(&packets, 1);

    // test buf_packet_t callback functions
    dplist_t * sent_packets = dpl_create(&buf_packet_copy, &buf_packet_free, &buf_packet_comp);
    for(i=0;i<5;i++)
    {
        header_t header;
        header.seq_num = htonl(i);  // host to network, unsigned int 32
        header.version = 'A';
        header.packet_type = i;
        header.ack_num = htonl((uint32_t)(i+20));
        header.magicnum = htons((uint32_t)(i * 3));
        header.packet_len = htons(110); // host to network, uint 16
        header.header_len = htons(520);
        
        data_packet_t *packet = malloc(sizeof(data_packet_t));
        packet->header = header;
        packet->data = malloc(DATALEN*sizeof(char));
        memcpy(packet->data, "abcdefg", DATALEN);
        
        buf_packet_t * buf_packet = malloc(sizeof(buf_packet_t));
        buf_packet->packet = *packet;
        buf_packet->ts = i*1000+1;

        printf("\n===================================\n");
        dpl_insert_at_index(sent_packets, buf_packet, 0, 1);
        buf_packet_t * tmp = dpl_get_element_at_index(sent_packets, 0);
        printf("Src data: ts=%ld, data=%s\n", buf_packet->ts, buf_packet->packet.data);
        print_header(buf_packet->packet.header);
        printf("===================================\n");
        printf("Copied: ts=%ld, data=%s\n", tmp->ts, tmp->packet.data);
        print_header(tmp->packet.header);
        printf("ts=%ld\n", tmp->ts);
        free(packet->data);
        free(packet);
        free(buf_packet);
    }
    dpl_free(&sent_packets, 1);

    return 0;

}


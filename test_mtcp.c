/* test my TCP implementation */
#include <stdlib.h>

#include "lib/mtcp.h"
#include "lib/dplist.h"
#include "lib/debug.h"

int main()
{
    dplist_t * packets = NULL;
    packets = dpl_create(&packet_copy, &packet_free, packet_comp);

    int i = 0;
    for(i=0;i<5;i++)
    {
        header_t header;
        header.seq_num = i;
        header.version = 'A';
        header.packet_type = i;
        header.ack_num = i+20;
        data_packet_t packet ;
        packet.header = header;
        packet.data = malloc(DATALEN*sizeof(char));
        memcpy(&packet.data, "abcdefg", DATALEN);

        dpl_insert_at_index(packets, &packet, 0, 1);
        data_packet_t * tmp = dpl_get_element_at_index(packets, 0);
        print_header(tmp->header);
        free(packet.data);
    }

    dpl_free(&packets, 1);
    return 0;

}


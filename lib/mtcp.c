#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mtcp.h"


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
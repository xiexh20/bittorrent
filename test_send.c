/** a file to test sending data using UDP
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


int main()
{
    // test the size of u_int
    u_int i = 10;
    uint16_t k = 20;
    uint32_t j = 30;
    printf("Size of u_int:%ld\n", sizeof(i));
    printf("Size of uint_16_t: %ld\n", sizeof(k));
    printf("size of uint32: %ld\n", sizeof(j));

}
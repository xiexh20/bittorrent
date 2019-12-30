/** file to test bt_parse */

#include <stdio.h>
#include <stdlib.h>
#include "lib/bt_parse.h"


int main(int argc, char **argv)
{
    bt_config_t config;

    bt_init(&config, argc, argv);

    bt_parse_command_line(&config);

    return 0;

}
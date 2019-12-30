#include "lib/input_buffer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void printline(char *line, void *cbdata)
{
    printf("LINE:  %s\n", line);
    printf("CBDATA:  %s\n", (char *)cbdata);
}

int main()
{

    struct user_iobuf *u;

    u = create_userbuf();
    assert(u != NULL);

    while (1)
    {
        process_user_input(STDIN_FILENO, u, printline, "Received!");
    }

    return 0;
}

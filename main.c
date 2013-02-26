#include  <unistd.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <errno.h>

#include    "parse.h"
#include    "draw.h"

int main(int argc, char *argv[])
{
    if(argc < 2) 
    {
        fprintf(stderr, "no input file given\n");
        _exit(0);
    }

    int fd = open(argv[1], O_RDONLY);
    dup2(fd, 0);

    struct MAIN *m = read_MAIN();
    start_draw(m);

    return 0;
}


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "simple.h"
#include "polyp-error.h"

#define BUFSIZE 1024

int main(int argc, char*argv[]) {
    static const struct pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };
    struct pa_simple *s = NULL;
    int ret = 1;
    int error;

    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, &error))) {
        fprintf(stderr, "Failed to connect to server: %s\n", pa_strerror(error));
        goto finish;
    }

    for (;;) {
        uint8_t buf[BUFSIZE];
        ssize_t r;
        
        if ((r = read(STDIN_FILENO, buf, sizeof(buf))) <= 0) {
            if (r == 0) /* eof */
                break;
            
            fprintf(stderr, "read() failed: %s\n", strerror(errno));
            goto finish;
        }

        if (pa_simple_write(s, buf, r, &error) < 0) {
            fprintf(stderr, "Failed to write data: %s\n", pa_strerror(error));
            goto finish;
        }
    }
    
    ret = 0;

finish:

    if (s)
        pa_simple_free(s);
    
    return ret;
}

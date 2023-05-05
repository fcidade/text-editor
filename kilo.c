#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

struct termios original_termios;

// Terminal

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode(void) {
    int err = tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
    if (err == -1) {
        die("tcsetattr");
    }
}

void enableRawMode(void) {
    int err = tcgetattr(STDIN_FILENO, &original_termios);
    if (err == -1) {
        die("tcgetattr");
    }
    atexit(disableRawMode);

    struct termios raw = original_termios;

    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK |ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    /* Sets a timeout for the read function  */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    err = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    if (err == -1) {
        die("tcsetattr");
    }
}

// Init

int main(void) {
    enableRawMode();

    while (1) {
        char c = '\0';
        int err = read(STDIN_FILENO, &c, 1);
        if (err == -1 && errno != EAGAIN) {
            die("read");
        }

        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }

        if (c == 'q') {
            break;
        }
    }
    return 0;
}

#include <termios.h>

void cfmakeraw(struct termios *tio)
{
    tio->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                      ICRNL | IXON);
    tio->c_oflag &= ~OPOST;
    tio->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tio->c_cflag &= ~(CSIZE | PARENB);
    tio->c_cflag |= CS8;
    tio->c_cc[VMIN] = 1;
    tio->c_cc[VTIME] = 0;
}

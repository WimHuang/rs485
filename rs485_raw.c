#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>

int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;

    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("get attr failed");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |=  CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch(nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch(nEvent)
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }

    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    case 921600:
        cfsetispeed(&newtio, B921600);
        cfsetospeed(&newtio, B921600);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }

    if (nStop == 1)
    {
        newtio.c_cflag &= ~CSTOPB;
    }
    else if (nStop == 2)
    {
        newtio.c_cflag |= CSTOPB;
    }

    newtio.c_cc[VTIME]  = 0;
    newtio.c_cc[VMIN] = 0;

    tcflush(fd,TCIFLUSH);

    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("set attr failed");
        return -1;
    }

    return 0;
}

int main(int argc, const char* argv[])
{
    int i, fd, len;
    char devbuf[32], recvbuf[32];
    char sendbuf[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b};

    if (4 != argc)
    {
        printf("Usage:  rs485_raw [uart port] [baud rate] [test interval ms]\n");
        printf("Sample: rs485_raw ttyO2 4800 1000\n");
        return 0;
    }

    sprintf(devbuf, "/dev/%s", argv[1]);

    fd = open(devbuf, O_RDWR);
    if ( fd == -1)
    {
        printf("Open %s faild\n", devbuf);
        exit(1);
    }
    printf("Open %s successfully\n", devbuf);

    if (set_opt(fd, atoi(argv[2]), 8, 'N', 1))
    {
        printf("Set %s faild\n", devbuf);
        exit(1);
    }
    printf("Set opt(%d 8 'N' 1) successfully\n", atoi(argv[2]));

    while (1)
    {
        len = write(fd, sendbuf, sizeof(sendbuf));
        if (len > 0)
        {
            printf("send data: ");
            for (i = 0; i < len; i++)
            {
                printf("0x%02x ", sendbuf[i]);
            }
            printf("\n");
        }

        // 20 bytes at 4800 baudrate
        usleep(100 * 1000);

        len = read(fd, recvbuf, 9);
        if (len > 0)
        {
            printf("recv data: ");
            for (i = 0; i < len; i++)
            {
                printf("0x%02x", recvbuf[i]);
            }
            printf("\n");
        }

        usleep(1000 * atoi(argv[3]));
    }

    close(fd);

    return 0;
}

extern "C"
{
#include "csapp.h"
}
//#include <opencv2/opencv.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
//#include <highgui.h>
#include <iostream>
//using namespace cv;
int main(int argc, char **argv)
{
    int clientfd, port;
    char *host;
    rio_t rio;

    if (argc != 4)
    {
        fprintf(stderr, "usage: %s <image_name> <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[2];
    port = atoi(argv[3]);
    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL)
    {
        printf("File open error");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);

    Rio_writen(clientfd, &size, sizeof(size));

    fseek(fp, 0, SEEK_SET);

    while (1)
    {

        unsigned char buff[256];
        int nread = fread(buff, 1, 256, fp);
        printf("File read :%d bytes\n", nread);

        if (nread > 0)
        {
            Rio_writen(clientfd, buff, nread);
            printf("Sending %d\n", nread);
        }

        if (nread < (256))
        {
            if (feof(fp))
                printf("End of file\n");
            if (ferror(fp))
                printf("Error reading\n");
            break;
        }
    }
    fclose(fp);

    int rx_fsize;
    int bytesReceived = 0;
    char recvBuff[256];
    Rio_readnb(&rio, &rx_fsize, sizeof(rx_fsize));
    //rx_fsize = atoi(recvBuff);
    printf("Size of rx file %d\n", rx_fsize);

    FILE *rx_fp;
    rx_fp = fopen("rx_image.jpg", "wb");

    if (NULL == rx_fp)
    {
        printf("Error opening file");
        return -1;
    }

    // fseek(fp, 0, SEEK_END);
    // int size = ftell(fp);
    fseek(rx_fp, 0, SEEK_SET);

    int recData = 256;

    // Receive data in chunks of 256 bytes untill rx_fsize data is received
    // while((bytesReceived = read(connfd, recvBuff, 256)) > 0)
    while (((bytesReceived = Rio_readnb(&rio, recvBuff, recData)) > 0) && rx_fsize > 0)
    {
        printf("Bytes received %d\n", bytesReceived);
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1, bytesReceived, rx_fp);
        // printf("%s \n", recvBuff);
        rx_fsize -= bytesReceived;
        if (rx_fsize < 256) //if remaining bytes is less than 256, read only remaining bytes of data
            recData = rx_fsize;
    }

    fclose(rx_fp);
    printf("File received\n");

    Close(clientfd); //line:netp:echoclient:close
    exit(0);
}

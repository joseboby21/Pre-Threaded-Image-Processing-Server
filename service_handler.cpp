
#include "csapp.h"
#include <stdio.h>
#include <iostream>
char imageRcvd = 1;
int processimage(int desc);
int desc = 0;

void service_handler(int connfd)
{
	desc = connfd;
	char buff[50];
	char buff1[50];

	snprintf(buff, 11, "image%d.jpg", connfd);
	size_t n;
	int bytesReceived = 0, result;
	char recvBuff[256];
	rio_t rio;

	Rio_readinitb(&rio, connfd);
	int rx_fsize;

	Rio_readnb(&rio, &rx_fsize, sizeof(rx_fsize));

	FILE *fp;
	fp = fopen(buff, "wb");
	if (NULL == fp)
	{
		printf("Error opening file");
		return;
	}

	fseek(fp, 0, SEEK_SET);
	int recData = 256;

	while (((bytesReceived = Rio_readnb(&rio, recvBuff, recData)) > 0) && rx_fsize > 0)
	{
		fwrite(recvBuff, 1, bytesReceived, fp);
		rx_fsize -= bytesReceived;
		if (rx_fsize < 256)
		{
			recData = rx_fsize;
		}
	}
	fclose(fp);
	printf("File received\n");

	processimage(desc);

	FILE *fp2;
	snprintf(buff1, 13, "image2_%d.jpg", connfd);
	fp2 = fopen(buff1, "rb");
	if (NULL == fp2)
	{
		printf("Error opening file");
		return;
	}
	fseek(fp2, 0, SEEK_END);
	int tx_size = ftell(fp2);
	fseek(fp2, 0, SEEK_SET);

	Rio_writen(connfd, &tx_size, sizeof(tx_size));
	fseek(fp2, 0, SEEK_SET);

	while (1)
	{
		unsigned char buff[256];
		int nread = fread(buff, 1, 256, fp2);
		if (nread > 0)
		{
			Rio_writen(connfd, buff, nread);
		}
		if (nread < (256))
		{
			if (feof(fp2))
			{
				printf("End of file\n");
			}
			if (ferror(fp2))
			{
				printf("Error reading\n");
			}
			break;
		}
	}
	fclose(fp2);
	return;
}

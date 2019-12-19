#include "csapp.h"
/*#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>*/
#define QUEUESIZE 6
pthread_t service1, service2, service3;

typedef struct
{
	int buf[QUEUESIZE];
	long head, tail;
	int full, empty;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull, *notEmpty;
} queue;

queue *fifo;

queue *queueInit(void);
void queueDelete(queue *q);
void queueAdd(queue *q, int in);
void queueDel(queue *q);
void millisleep(int milliseconds);

void service_handler(int connfd);
void *consumer(void *args);

int main(int argc, char **argv)
{
	struct sched_param param;
	pthread_attr_t service_attr;

	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	int max_priority = param.sched_priority;
	pthread_setschedparam(pthread_self(), SCHED_RR, &param);
	param.sched_priority = max_priority--;
	pthread_attr_init(&service_attr);
	pthread_attr_setschedparam(&service_attr, &param);

	int listenfd, connfd, port;
	unsigned int clientlen;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp;

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	fifo = queueInit();
	if (fifo == NULL)
	{
		fprintf(stderr, "main: Queue Init failed.\n");
		exit(1);
	}

	pthread_create(&service1, &service_attr, consumer, NULL);
	pthread_create(&service2, &service_attr, consumer, NULL);
	pthread_create(&service3, &service_attr, consumer, NULL);

	port = atoi(argv[1]);
	listenfd = Open_listenfd(port);
	while (1)
	{
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		printf("Producer Thread: Request Received.\n");
		pthread_mutex_lock(fifo->mut);
		while (fifo->full)
		{
			printf("producer: queue FULL.\n");
			pthread_cond_wait(fifo->notFull, fifo->mut);
		}
		queueAdd(fifo, connfd);
		pthread_mutex_unlock(fifo->mut);
		pthread_cond_signal(fifo->notEmpty);
		printf("Producer Thread: Inserted into the buffer.\n");
	}
	Close(listenfd);
	exit(0);
}

void *consumer(void *args)
{
	while (1)
	{
		int connfd = 0;
		pthread_mutex_lock(fifo->mut);
		while (fifo->empty)
		{
			printf("Worker Thread %d : queue EMPTY.\n", pthread_self());
			pthread_cond_wait(fifo->notEmpty, fifo->mut);
		}
		printf("Worker Thread %d: request serving started.\n", pthread_self());
		connfd = fifo->buf[fifo->head];
		queueDel(fifo);
		pthread_mutex_unlock(fifo->mut);
		pthread_cond_signal(fifo->notFull);
		millisleep(30000);
		service_handler(connfd);
		Close(connfd);
	}
}

queue *queueInit(void)
{
	queue *q;

	q = (queue *)malloc(sizeof(queue));
	if (q == NULL)
		return (NULL);

	q->empty = 1;
	q->full = 0;
	q->head = 0;
	q->tail = 0;
	q->mut = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(q->mut, NULL);
	q->notFull = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	pthread_cond_init(q->notFull, NULL);
	q->notEmpty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	pthread_cond_init(q->notEmpty, NULL);

	return (q);
}

void queueDelete(queue *q)
{
	pthread_mutex_destroy(q->mut);
	free(q->mut);
	pthread_cond_destroy(q->notFull);
	free(q->notFull);
	pthread_cond_destroy(q->notEmpty);
	free(q->notEmpty);
	free(q);
}

void queueAdd(queue *q, int in)
{
	q->buf[q->tail] = in;
	q->tail++;
	if (q->tail == QUEUESIZE)
		q->tail = 0;
	if (q->tail == q->head)
		q->full = 1;
	q->empty = 0;

	return;
}

void queueDel(queue *q)
{
	q->head++;
	if (q->head == QUEUESIZE)
		q->head = 0;
	if (q->head == q->tail)
		q->empty = 1;
	q->full = 0;

	return;
}

void millisleep(int milliseconds)
{
	usleep(milliseconds * 1000);
}

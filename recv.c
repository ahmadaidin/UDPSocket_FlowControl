/*
* File : receiver.c
*/
#include "dcomm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>


/* Delay to adjust speed of consuming buffer, in milliseconds */
#define DELAY 500

/* Define receive buffer size */
#define RXQSIZE 8

Byte rxbuf[RXQSIZE+1];
QTYPE rcvq = { 0, 0, 0, RXQSIZE, rxbuf };
QTYPE *rxq = &rcvq;
Byte sent_xonxoff = XON;
bool send_xon = false,
send_xoff = false;
Byte buf[1];
int recvlen;
Byte dat;

/* Socket */
int sockfd; // listen on sockfd
struct sockaddr_in myaddr, remaddr;	/* our address */
unsigned int addrlen;	/* length of address (for getsockname) */
int rc;
int j=1;

/* Functions declaration */

static Byte *rcvchar(int sockfd, QTYPE *queue);
static Byte *q_get(QTYPE *, Byte *);
void *Consume();
void add(QTYPE *queue, Byte x);
void del(QTYPE *queue, Byte *b);
// MAIN PROGRAM
int main(int argc, char *argv[])
{
	pthread_t thread;
  pthread_attr_t attr;
	Byte c;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("cannot create socket\n");
		return 0;
	}

	// Binding
	printf("Binding pada 127.0.0.1:%s ...\n", argv[1]);
	int port = atoi(argv[1]);
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(myaddr.sin_addr.s_addr));
	myaddr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}

	memset((char *)&remaddr, port, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(port);
	if(inet_pton(AF_INET,"127.0.0.1", &(myaddr.sin_addr.s_addr))==0){
		fprintf(stderr, "inet_aton failed\n" );
		exit(1);
	}


	/* Create child process */
	rc = pthread_create(&thread, NULL, &Consume, NULL);
  if (rc) {
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
  }


	/*** IF PARENT PROCESS ***/
	int i=1;
	while (true) {
		c = *(rcvchar(sockfd, rxq));
		/* Quit on end of file */
		if (c == EOF) {
			exit(0);
		} else{
			printf("Menerima byte ke-%d\n", i);
		}
		++i;
	}
	return 0;
}

static Byte *rcvchar(int sockfd, QTYPE *queue) {
	int upperLimit=6;
	int slen=sizeof(remaddr);
	recvlen=recvfrom(sockfd, buf,1, 0, (struct sockaddr *)&remaddr, &slen);
	if(recvlen>0){
		if((queue->count)<=upperLimit){
			add(queue, buf[0]);
			return &buf[0];
		} else {
			printf("Buffer > minimum upperlimit. Mengirimkan XOFF.\n");
			buf[0]=XOFF;
			if(sendto(sockfd, buf,1,MSG_MORE,(struct sockaddr *)&remaddr, sizeof(remaddr))==-1) {
        perror("sendto");
        exit(1);
      }
			return 0;
		}
	}
	return &(queue->data[queue->rear]);
}

static Byte *q_get(QTYPE *queue, Byte *data)
{
	int lowerLimit = 2;
	Byte *current;
	/* Nothing in the queue */
	if (!queue->count) return (NULL);

		del(queue, current);
		*data = *current;
		printf("Mengkonsumsi byte ke-%d: '%c'\n", j, *current);
		if (queue->count < lowerLimit)
		{
			printf("Buffer < maximum lowerlimit. Mengirimkan XON.\n");
			buf[0]=XON;
			if(sendto(sockfd, buf,1,0,(struct sockaddr *)&remaddr, sizeof(remaddr))==-1) {
        perror("sendto");
        exit(1);
      }
		}
		++j;

	return current;
}

void *Consume(){
	printf("Consume\n");
	while (true){
		printf("ooooo\n");
		Byte *b=q_get(rxq, &dat);
		sleep(2);
	}
}

void add(QTYPE *queue, Byte x)
{
	if((queue->front ==0) && (queue->rear == 0))
	{
		queue->front=queue->rear+1;
		queue->count++;
	}
	else if(queue->front == (queue->rear+1)%(queue->front)){
		printf("Circular Queue over flow");
	}
  queue->rear= (queue->rear+1)%(queue->maxsize);
	queue->data[queue->rear]=x;
}

void del(QTYPE *queue, Byte *b)
{
	if(queue->front==0 && queue->rear == 0)
	{
		printf("under flow");
		return;
	}
	else if(( queue->front== queue->rear  ) && queue->front!=0)
	{
		*b=queue->data[queue->front];
		queue->front=queue->rear=0;
	} else{
		*b=queue->data[queue->front];
	}
	queue->front= (queue->front+1)%(queue->maxsize);
}

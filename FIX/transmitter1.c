// Nama file: transmitter1.c
// Tugas Kecil 1 IF3130 Jaringan Komputer "Flow Control"
// K02
// Ahmad Aidin (13513020) dan Ryan Yonata (13513074)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "dcomm.h"
#include <stdbool.h>

#define DELAY 500

// Variabel
Byte lastMSG;
Byte buf[1];
int rc;
int recvlen;
int sockfd;
struct sockaddr_in myaddr, remaddr;
int slen=sizeof(remaddr);

// Deklarasi Fungsi
static void *RecvFrom();

// MAIN PROGRAM
int main(int arg, char*  argv[])
{
	// Variabel
	unsigned int addrlen;
	Byte c;

	int r;
	int i;
	pthread_t thread;
	pthread_attr_t attr;
	FILE *fp;

	// Membuka file txt
	fp=fopen(argv[3],"r");
	if(fp==NULL)
  	{
    	printf("cannot open file\n");
    	return 1;
  	}

  	//  Membuat socket
  	printf("Membuat socket untuk koneksi ke %s:%s ...\n", argv[1],argv[2]);
  	if((sockfd=socket(AF_INET, SOCK_DGRAM, 0)) <0 )
  	{
    	perror("cannot create socket\n");
    	return 0;
  	}

  	// Binding
  	int port = atoi(argv[2]);
  	memset((char *)&myaddr, 0, sizeof(myaddr));
  	myaddr.sin_family = AF_INET;
  	inet_pton(AF_INET, argv[1], &(myaddr.sin_addr.s_addr));
  	myaddr.sin_port = htons(0);

  	if (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) 
  	{
    	perror("bind failed");
    	return 0;
  	}

  	// Definisikan Remote address untuk mengirim karakter pada file txt
  	memset((char *)&remaddr, 0, sizeof(remaddr));
  	remaddr.sin_family = AF_INET;
  	remaddr.sin_port = htons(port);
  	if(inet_pton(AF_INET, argv[1], &(myaddr.sin_addr.s_addr))==0) 
  	{
    	fprintf(stderr, "inet_aton failed\n");
    	exit(1);
  	}

  	// Membuat thread untuk proses recvfrom
  	rc = pthread_create(&thread, NULL, &RecvFrom, NULL);
  	if(rc) 
  	{
    	printf("ERROR; return code from pthread_create() is %d\n", rc);
    	exit(-1);
  	}

  	// Proses utama, mengirim karakter ke transmitter
  	i=1;
  	do
  	{
    	if(lastMSG!=XOFF)
    	{
      		r=fscanf(fp, "%c", &c);
      		buf[0]=c;
      		printf("Mengirim byte ke-%d: '%c'\n", i, c);
      		if(sendto(sockfd, buf,1,0,(struct sockaddr *)&remaddr, sizeof(remaddr))==-1)
      		{
        		perror("sendto");
        		exit(1);
      		}
      		++i;
    	}
    	else // Jika XOFF
    	{
    		sleep(1);
      		printf("Menunggu XON...\n");
    	}
      	sleep(1);
  	} while(c!='0');

  	fclose(fp);
  	close(sockfd);
  	return 0;
}

// Prosedur recvfrom menerima sinyal XON atau XOFF dari receiver untuk
// menentukan proses yang dilakukan (mengirim/menunggu)
static void *RecvFrom()
{
    do
    {
        recvlen = recvfrom(sockfd, buf, 1, 0, (struct sockaddr *)&remaddr, &slen);
        if(buf[0]==XOFF)
        {
          printf("XOFF diterima.\n");
        } 
        else 
        {
          printf("XON diterima.\n");
        }
        lastMSG=buf[0];
        sleep(1);
    } while(true);
    printf("connection is terminated\n" );
}

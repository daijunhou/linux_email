#include <sys/types.h>      
#include <sys/socket.h>      
#include <netinet/in.h>      
#include <netdb.h>      
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>       
//using namespace std;      
#define EHLO "EHLO 163.com\r\n" //***为邮箱用户名     
#define DATA "data\r\n"      
#define QUIT "QUIT\r\n" 
 
//#define h_addr h_addr_list[0]     
//FILE *fin;     
int sock;     
struct sockaddr_in server;     
struct hostent *hp, *gethostbyname();     
char buf[BUFSIZ+1];     
int len;     
char *host_id="smtp.163.com";     
char *from_id="18550200304@163.com";     
char *to_id="1102214616@qq.com";     
char *sub="Monitormail\r\n";     
char wkstr[100]="it is for test monitor\r\n";      
 
/*=====Send a string to the socket=====*/      
void send_socket(char *s)     
{         
	write(sock,s,strlen(s));         
	//write(1,s,strlen(s));         
	//printf("Client:%s\n",s);     
}      
 
//=====Read a string from the socket=====*/      
void read_socket()     
{
	len = read(sock,buf,BUFSIZ);
	write(1,buf,len);
	printf("Server:%s\n",buf);     
}      
 
/*=====MAIN=====*/     
int main(int argc, char* argv[])     
{      
 
	/*=====Create Socket=====*/
	sock = socket(AF_INET, SOCK_STREAM, 0);     
	if (sock==-1)     
	{
		perror("opening stream socket");
		//exit(1);  
		return 1;   
	}     
	else       
		//cout << "socket created\n";
		printf("socket created\n");
 
	/*=====Verify host=====*/
	server.sin_family = AF_INET;
	hp = gethostbyname(host_id);
	if (hp==(struct hostent *) 0)
	{
		fprintf(stderr, "%s: unknown host\n", host_id);
		//exit(2);
		return 2;
	}
	printf("----------------------\n"); 
	/*=====Connect to port 25 on remote host=====*/
	memcpy((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);     	 				
	server.sin_port=htons(25); /* SMTP PORT */
	if (connect(sock, (struct sockaddr *) &server, sizeof server)==-1)
	{
		perror("connecting stream socket");
		//exit(1);
		return 1;
	}     
	else       
		//cout << "Connected\n";
		printf("Connected\n");
 
	/*=====Write some data then read some =====*/
	read_socket(); /* SMTP Server logon string */
	send_socket(EHLO); /* introduce ourselves */
	read_socket(); /*Read reply */
 
	/*
	**added by fupeng
	*/
	send_socket("AUTH LOGIN");
	send_socket("\r\n");
	read_socket();
	send_socket("MTg1NTAyMDAzMDQ=");//用户名的base64编码
	send_socket("\r\n");
	read_socket();
	send_socket("MXFhejJ3c3g=");//密码的base64编码
	send_socket("\r\n");
	read_socket();
 
	send_socket("mail from <");
	send_socket(from_id);
	send_socket(">");
	send_socket("\r\n");
	read_socket(); /* Sender OK */
 
	//send_socket("VRFY ");
	//send_socket(from_id);
	//send_socket("\r\n");     
	//read_socket(); // Sender OK */
	send_socket("rcpt to <"); /*Mail to*/
	send_socket(to_id);
	send_socket(">");
	send_socket("\r\n");
	read_socket(); // Recipient OK*/
 
	send_socket(DATA);// body to follow*/
	read_socket(); 
	
	send_socket("subject:");
	send_socket(sub);
	//read_socket(); // Recipient OK*/
	send_socket("from:18550200304@163.com");
	//read_socket(); // Recipient OK*/
	send_socket("\r\n\r\n");
	send_socket(wkstr);
	//read_socket();
	//send_socket("\r\n");
	send_socket(".\r\n");
	read_socket();
	send_socket(QUIT); /* quit */
	read_socket(); // log off */
 
	//=====Close socket and finish=====*/
	close(sock);
	//exit(0);
	return 0;
} 

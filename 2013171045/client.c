#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
void *receiver(void *psockfd);
int main() {

	int sockfd;
	char buffer[1000];
	ssize_t n;
	pthread_t client_thread;

	struct sockaddr_in servaddr;

//////Create Client Socket///////
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) perror("Can not create a Socket!!\n");
/////////////////////////////////




/////// IP Address �� Port�� �Է� ���� ���� ///////////
	char ipadd[20];
	char portstr[10];
	int port;
/////////////////////////////////////////////////////



	printf("Input Server IP Address : ");
	scanf("%s", ipadd);

////// Return -1 if IP Address is not "127.0.0.1" ////
	if (strcmp(ipadd, "127.0.0.1")){
		printf("Invalid IP Address!\n");
		return -1;
	}
////////////////////////////////////////////////////


////// Get port number //////
	printf("Input Server Port Number : ");
	scanf("%s", portstr);
	port = atoi(portstr);
/////////////////////////////

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ipadd);
	servaddr.sin_port = htons(port);

	printf("open!! client\n");


//////Connect to server///////////////
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
		perror("Error ");
		return -1;
	}
	printf("Success to connect to server!\n");
	printf("Server : welcome to chatting server!!\n");
	printf(" Chatting on...\n");
	printf("input 'Q' to exit\n");
////////////////////////////////////////////


	int *psockfd = &sockfd;
	pthread_create(&client_thread, NULL, receiver, (void*)psockfd);

	while (1) {
		memset(&buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		int len = strlen(buffer);
		buffer[len - 1] = 0;


////////// send ȣ�� �� ���������� ȣ�� �Ǿ��� �� ���� �޽��� ��� �� ���� ////
		if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
			printf("Error \n");
			return 1;
		}
////////////////////////////////////////////////////////////////////
		
		
		
////////// 'Q' �� �Է¹޾��� ��� Client �� ������ /////////////////	
		if(!strcmp(buffer, "Q")){
			printf("Disconnected\n");
			return 0;
		}
/////////////////////////////////////////////////////////////////


	}
	close(sockfd);
	return 0;

}



//// thread �� ������ �� ����� �Լ� //////////
void *receiver(void *psockfd) {
	char server_reply[2000];// �������� ���� �޽����� ������ ����

	int sockfd = *(int*)psockfd;
	while (recv(sockfd, server_reply, 2000, 0) > 0) {// ���������� receive �Ͽ��� ��� �޽��� ���
		printf("%s\n", server_reply);
		memset(&server_reply, 0, sizeof(server_reply));// ���۸� û����
	}

}

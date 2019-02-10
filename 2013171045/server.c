#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

int ind = 0;
int connfdarr[10];
void *server_handler(void *fd_pointer);
char notice[100] = "Client 0 has been connected!";


int connection[10];//1은 연결된 상태, 0은 연결 되지 않은 상태


//thread를 구축 할 때에 사용할 함수인 handler에 들어갈 input struct
typedef struct {
	int connfd;//accept의 return 값
	int currind;//Client의 index
} hin;


int main() {


//// port를 입력받을 때 사용할 변수 ////
	char portstr[5];
	int port;
//////////////////////////////////////


	int listenfd, connfd, *new_sock;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

////// Create Socket //////
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) perror("Can not create a Socket!!\n");
///////////////////////////




////// Get port number //////
	printf("Server Port : ");
	scanf("%s", portstr);
	port = atoi(portstr);
///////////////////////////

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);


////// Bind the socket to address/////////////
	if (bind(listenfd, (struct sockaddr *) &servaddr,
		sizeof(servaddr)) < 0) {
		perror("ERROR on binding");
		return 1;
	}
///////////////////////////////////////////////


	printf("open!! server\n");
	printf("Chatting on\n");

	listen(listenfd, 10);
	clilen = sizeof(cliaddr);
	
	while (connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) {
		
		if (connfd < 0) {
			perror("Accept Failed\n");
			return -1;
		}




/////////// Client 의 index가 10에 도달하였을 경우 ///////////////
/////////// 또는 한바퀴 돌고난 후 들어가려는 index가 차있을 경우 ///
		int nine=0;
		if (ind == 10 || connection[ind]==1) {
			for (int i = 0; i < 10; i++) {
				if (connection[i] == 0) {
					ind = i;// 앞에서부터 빈 곳에 채워 넣음
					connection[i] = 1;//연결 됐으므로 connection을 1로
					if (i == 9)	nine = 1;// i==9일 때, connection을 만족했다면 nine=1로 설정
					break;
				}
				if (i == 9 && nine == 0) {// i==9까지 검사를 하였지만 모든 인덱스가 차 있을 경우
					printf("Cannot make more connection!!\n");
					continue;
				}
				nine = 0;//다시 0으로 돌려 놓음
			}
		}
/////////////////////////////////////////////////////////////////////////////

		


		connection[ind] = 1;//연결에 성공 했다면 connection을 1로 설정
		
		connfdarr[ind] = connfd;
		printf("Connected to Client %d\n", ind);



		notice[7] = ind + '0';



		for (int i = 0; i < 10; i++) {
			if (connection[i] == 1 && i != ind) {
				write(connfdarr[i], notice, strlen(notice));
			}
		}


		pthread_t server_thread[10];//maximum # of thread to be 10
		new_sock = malloc(1);
		hin *new;
		new->connfd = connfd;
		new->currind = ind;
		pthread_create(&server_thread[ind], NULL, server_handler, (void*)new);
		ind++;
	}


	return 0;

}


//// thread 를 생성할 때 사용할 함수 //////////
void *server_handler(void *fd_pointer) {

	int sock = ((hin *)fd_pointer)->connfd;
	int currind = ((hin *)fd_pointer)->currind;
	static char client_message[2000];//client로 부터 message를 받을 버퍼

	
	while ((recv(sock, client_message, 2000, 0)) > 0) {
		

/////////// Client가 'Q'를 입력했을 경우 //////////////
		if (strcmp(client_message, "Q") == 0) {
			connection[currind] = 0;
			printf("Client %d disconnected\n", currind);
			continue;
		}
/////////////////////////////////////////////////////////




//////////// client로 부터 받은 message를 "Client x : ~~~" 의 형태로 바꿈///
		char clientnumber[2010] = "Client 0 : ";
		clientnumber[7] = (char)currind + '0';
		strcat(clientnumber, client_message);
////////////////////////////////////////////////


		printf("message from %s\n", clientnumber);
		for (int i = 0; i < 10; i++) {
			if (connection[i]==1 && i != currind) {//연결이 끊긴 client와 message를 보낸 client를 제외
				write(connfdarr[i], clientnumber, strlen(clientnumber));
			}
		}
		memset(&client_message, 0, sizeof(client_message));//버퍼를 청소해줌
	}

	
	return 0;

}
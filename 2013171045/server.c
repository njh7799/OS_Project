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


int connection[10];//1�� ����� ����, 0�� ���� ���� ���� ����


//thread�� ���� �� ���� ����� �Լ��� handler�� �� input struct
typedef struct {
	int connfd;//accept�� return ��
	int currind;//Client�� index
} hin;


int main() {


//// port�� �Է¹��� �� ����� ���� ////
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




/////////// Client �� index�� 10�� �����Ͽ��� ��� ///////////////
/////////// �Ǵ� �ѹ��� ���� �� ������ index�� ������ ��� ///
		int nine=0;
		if (ind == 10 || connection[ind]==1) {
			for (int i = 0; i < 10; i++) {
				if (connection[i] == 0) {
					ind = i;// �տ������� �� ���� ä�� ����
					connection[i] = 1;//���� �����Ƿ� connection�� 1��
					if (i == 9)	nine = 1;// i==9�� ��, connection�� �����ߴٸ� nine=1�� ����
					break;
				}
				if (i == 9 && nine == 0) {// i==9���� �˻縦 �Ͽ����� ��� �ε����� �� ���� ���
					printf("Cannot make more connection!!\n");
					continue;
				}
				nine = 0;//�ٽ� 0���� ���� ����
			}
		}
/////////////////////////////////////////////////////////////////////////////

		


		connection[ind] = 1;//���ῡ ���� �ߴٸ� connection�� 1�� ����
		
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


//// thread �� ������ �� ����� �Լ� //////////
void *server_handler(void *fd_pointer) {

	int sock = ((hin *)fd_pointer)->connfd;
	int currind = ((hin *)fd_pointer)->currind;
	static char client_message[2000];//client�� ���� message�� ���� ����

	
	while ((recv(sock, client_message, 2000, 0)) > 0) {
		

/////////// Client�� 'Q'�� �Է����� ��� //////////////
		if (strcmp(client_message, "Q") == 0) {
			connection[currind] = 0;
			printf("Client %d disconnected\n", currind);
			continue;
		}
/////////////////////////////////////////////////////////




//////////// client�� ���� ���� message�� "Client x : ~~~" �� ���·� �ٲ�///
		char clientnumber[2010] = "Client 0 : ";
		clientnumber[7] = (char)currind + '0';
		strcat(clientnumber, client_message);
////////////////////////////////////////////////


		printf("message from %s\n", clientnumber);
		for (int i = 0; i < 10; i++) {
			if (connection[i]==1 && i != currind) {//������ ���� client�� message�� ���� client�� ����
				write(connfdarr[i], clientnumber, strlen(clientnumber));
			}
		}
		memset(&client_message, 0, sizeof(client_message));//���۸� û������
	}

	
	return 0;

}
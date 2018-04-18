#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h> /* Biblioteca para operaçoes com diretorios */

#include <netinet/in.h> /* Biblioteca contendo endereços de dominios da internet */
#include <sys/socket.h> /* Biblioteca contendo as definicoes de sockets */
#include <arpa/inet.h> /* Biblioteca contendo Funcoes referentes ao inet (Rede) */

#define MAXPENDING 1 /* Tamanho maximo da fila de conexoes pendentes */
#define BUFFSIZE 512 /* Tamanho de buffer */
#define PORT 12345 /* Porta utilizada pelo server */
#define DIRETORIO "/home/claudio" /* Diretorio onde ficam os arquivos */
#define IS_FILE 0x8


void Die(char *mess) { perror(mess); exit(1); }

int main(){
	DIR *dir;
	FILE *arq;
	int i, j, tam;
	int serversock, clientsock;
	struct sockaddr_in echoserver, echoclient;
	struct dirent *pDirent; //struct para o diretorio

	char buffer[BUFFSIZE];
	char caracter;

	/* limpar a tela */
	system("clear");

	printf("Iniciando servidor na porta: %d \n", PORT);

	/* criar socket TCP */
	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Falha ao criar socket");
	}

	/* inicia a struct sockaddr_in do servidor */
	memset( & echoserver, 0, sizeof(echoserver)); /* limpa a struct */
	echoserver.sin_family = AF_INET; /* Internet/IP */
	echoserver.sin_addr.s_addr = htonl(INADDR_ANY); /* Incoming addr */
	echoserver.sin_port = htons(PORT); /* porta do servidor */

	/* inicia socket em um endereço */
	if (bind(serversock, (struct sockaddr * ) & echoserver, sizeof(echoserver)) < 0) {
	  Die("Falha ao iniciar o socket");
	}
	 /* escutar conexoes do socket */
	if (listen(serversock, MAXPENDING) < 0) {
	 Die("Falha ao escutar o socket");
	}

	while (1){    
		unsigned int clientlen = sizeof(echoclient);

		char *file = (char*)calloc(100, sizeof(char));

		if ((clientsock = accept(serversock, (struct sockaddr * ) & echoclient, & clientlen)) < 0){
			Die("Falha ao aceitar a conexao do cliente");
		}   
	    
		if (clientsock > 0){    
			printf("Cliente conectado: %s\n", inet_ntoa(echoclient.sin_addr));
		}
	        
		recv(clientsock, buffer, BUFFSIZE, 0); 

		for(i = 0; buffer[i] != ' '; i++);
		for(j = 0, i = i + 1; buffer[i] != ' '; i++, j++)
			file[j] = buffer[i];

		memset(&buffer, 0, sizeof(buffer));

		char diretorio[] = DIRETORIO;
		strcat(diretorio, file);
		printf("acessando: %s\n\n", diretorio);

		if(strstr(diretorio, ".txt")){
			if(arq = fopen(diretorio, "r")){
				i = 0;
				while( (caracter = fgetc(arq) )!= EOF){
					buffer[i] = caracter;
					i++;
				}
				write(clientsock, buffer, strlen(buffer));
				write(clientsock, "\n\n200 OK", 8);	
			}else
				write(clientsock, "400 bad request", 15);

		}else{
			if(dir = opendir(diretorio)){
				while ((pDirent = readdir(dir)) != NULL){
					if(pDirent->d_type == IS_FILE){
			            write(clientsock, pDirent->d_name, strlen(pDirent->d_name));
			            write(clientsock, "\n", 1);
			        }
				}
				write(clientsock, "\n\n200 OK", 8);
       		}else
       			write(clientsock, "400 bad request", 15);
		}
		close(clientsock);   
   }  
   return 0;
}


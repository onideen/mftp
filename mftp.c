#include "mftp.h"

void ftpClient() {
    int control_socket = 0, n = 0;
    char recvBuff[1024], sendBuff[1024];
    struct sockaddr_in server_address; 

    memset(recvBuff, '0',sizeof(recvBuff));
    memset(sendBuff, '0',sizeof(sendBuff));
    if((control_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        exit(0);
    } 

    memset(&server_address, '0', sizeof(server_address)); 

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(gArgs.port); 

    if(inet_pton(AF_INET, gArgs.hostname, &server_address.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        exit(0);
    } 
    if( connect(control_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
       printf("\n Error : Connect Failed \n");
       exit(0);
    } 

    while ( (n = read(control_socket, recvBuff, sizeof(recvBuff)-1)) > 0) {
        	
        recvBuff[n] = 0;
        


        if(fputs(recvBuff, stdout) == EOF) {
            printf("\n Error : Fputs error\n");
        }
		if (strncmp(recvBuff, "220", 3) == 0) {
			strcpy(sendBuff, "USER ");
			strcat(sendBuff, gArgs.username);
			write(control_socket, sendBuff, strlen(sendBuff));
		}


    } 

    if(n < 0) {
        printf("\n Read error \n");
    } 

    exit(0);
}

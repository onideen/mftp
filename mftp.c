#include "mftp.h"

static const BUFFER_SIZE = 1024;



void ftpClient() {
    int control_socket = 0, filesocket = 0;
    char recvBuff[BUFFER_SIZE], sendBuff[BUFFER_SIZE], fileRecv[BUFFER_SIZE];
    FILE *p = NULL;

    memset(recvBuff, '0',sizeof(recvBuff));
    memset(sendBuff, '0',sizeof(sendBuff));

    control_socket = connectSocket(gArgs.port);
    
    authentificate(control_socket, recvBuff, sendBuff);

    strcpy(sendBuff, "SYST\r\n");
    write(control_socket, sendBuff, strlen(sendBuff));

    printread(control_socket, recvBuff);

    // SET TYPE TO ASCII OR BINARY
    if (strcmp(gArgs.mode, "binary") == 0 ) {
        strcpy(sendBuff, "TYPE I\r\n");
        write(control_socket, sendBuff, strlen(sendBuff));
    } 

    printread(control_socket, recvBuff);

    if (strncmp(recvBuff, "200", 3) == 0) {
    // Mode successfully set 
    }



    if (gArgs.active) {
        //FIX ACTIVE MODE
    } else {
        /* PASV mode */
        int port;
        strcpy(sendBuff, "PASV\r\n");
        write(control_socket, sendBuff, strlen(sendBuff));
        printread(control_socket, recvBuff);
        port = findPasvPort(recvBuff);
        filesocket = connectSocket(port);

    }



    //etriveFile();
    strcpy(sendBuff, "RETR ");
    strcat(sendBuff, gArgs.downloadFile);
    strcat(sendBuff, "\r\n");
    write(control_socket, sendBuff, strlen(sendBuff));
    printread(control_socket, recvBuff);

    printread(filesocket, r);
    printread(control_socket, recvBuff);

    p = fopen(gArgs.downloadFile, "w");
    if (p== NULL) {
        printf("Error in opening a file..", gArgs.downloadFile);
    }
    fwrite(recvBuff, strlen(recvBuff), 1, p);
    fclose(p);


//    if (strncmp(recvBuff, "150  ", 3) == 0)

    exit(0);

}

int findPasvPort(char searchString[]) {
    char b[100];
    char e[100];
    char tmp[10];
    int offset;
    int secondlast, last, port;

    memset(b, '0',sizeof(b));
    memset(e, '0',sizeof(b));

    substrafter(b, searchString, ",", 4);
    substrafter(e, searchString, ",", 5);

    offset = strlen(b)-strlen(e)-1;

    b[offset] = 0;
    secondlast = atoi(b);

    substrafter(b, searchString, ",", 5);
    substrafter(e, searchString, ")", 1);

    offset = strlen(b)-strlen(e)-1;

    b[offset] = 0;
    last = atoi(b);

    port = 256*secondlast+last;
}

/* Returns 0 if succeded the authentification, -1 if error */ 
int authentificate(int socket, char recvBuff[], char sendBuff[]){
    int n = 0;
    printf("In authentificate: %d\n", sizeof(recvBuff));
    printread(socket, recvBuff);
 
    if (strncmp(recvBuff, "220", 3) == 0) {
        strcpy(sendBuff, "USER ");
        strcat(sendBuff, gArgs.username);
        strcat(sendBuff, "\r\n");
        write(socket, sendBuff, strlen(sendBuff));
    }
    printread(socket, recvBuff);

    if (strncmp(recvBuff, "331", 3) == 0) {
        strcpy(sendBuff, "PASS ");
        strcat(sendBuff, gArgs.password);
        strcat(sendBuff, "\r\n");
        write(socket, sendBuff, strlen(sendBuff));
    }
    
    printread(socket, recvBuff);
    
    if (strncmp(recvBuff, "230", 3) == 0) {
        return 0;        
    }else if (recvBuff, "530"){
        pdie(2);
    }
}

void printread(int socket, char recvBuff[]){
    int n;

    n = read(socket, recvBuff, BUFFER_SIZE-1); 
        
    if(n < 0) {
        printf("\n Read error \n");
        pdie(7);
    }
    
    recvBuff[n] = 0;

    if(fputs(recvBuff, stdout) == EOF) {
        printf("\n Error : Fputs error\n");
        pdie(7);
    }
}

int connectSocket(int port) {
    int socket_fd = 0;
    struct sockaddr_in server_address; 

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        exit(0);
    } 

    memset(&server_address, '0', sizeof(server_address)); 

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port); 

    if(inet_pton(AF_INET, gArgs.hostname, &server_address.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        exit(0);
    } 
    if( connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
       pdie(1);
    }

    return socket_fd;
}

void substrafter(char *out, char haystack[], char needle[], int nr) {
    int i;
    char *tmp;

    tmp = strstr(haystack , needle);
    for (i = 0; i < nr - 1; i++) {
        tmp = strstr(&tmp[1], needle);
    }
    strcpy(out, &tmp[1]);
}

void retriveFile(){
}
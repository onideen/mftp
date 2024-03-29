#include "mftp.h"

static const BUFFER_SIZE = 1024;
/**** !!!!!!!!!!!!!!!!!! KAN SPARE MANGE LINJER 
***************** sprintf(str, "%d", aInt);
**/
void *ftpClient(void * ftpConfvoid) {
    int control_socket = 0, i;
    char recvBuff[BUFFER_SIZE], sendBuff[BUFFER_SIZE];
     struct ftpArgs_t *ftpConf;

    ftpConf = (struct ftpArgs_t *)ftpConfvoid;

    memset(recvBuff, '0', sizeof(recvBuff));
    memset(sendBuff, '0', sizeof(sendBuff));
  
    control_socket = connectSocket(ftpConf->hostname, ftpConf->port);
    
    authentificate(ftpConf, control_socket, recvBuff, sendBuff);

    strcpy(sendBuff, "SYST\r\n");
    logWrite(control_socket, sendBuff);

    logRead(control_socket, recvBuff);

    setType(control_socket, sendBuff);

    logRead(control_socket, recvBuff);

    retriveFile(ftpConf, sendBuff, recvBuff, control_socket);
    logRead(control_socket, recvBuff);

}

int findPasvPort(char searchString[]) {
    char b[100];
    char e[100];
    char tmp[10];
    int offset;
    int secondlast, last, port;

    memset(b, '0',sizeof(b));
    memset(e, '0',sizeof(e));

    substrafter(b, searchString, ',', 4);
    substrafter(e, searchString, ',', 5);

    offset = strlen(b)-strlen(e)-1;

    b[offset] = 0;
    secondlast = atoi(b);

    substrafter(b, searchString, ',', 5);
    substrafter(e, searchString, ')', 1);

    offset = strlen(b)-strlen(e)-1;

    b[offset] = 0;
    last = atoi(b);

    port = 256*secondlast+last;
}

/* Returns 0 if succeded the authentification, -1 if error */ 
void authentificate(struct ftpArgs_t *ftpConf, int socket, char recvBuff[], char sendBuff[]){
    int n = 0, i;
    logRead(socket, recvBuff);
    char tmp[BUFFER_SIZE];
    char *ptr;
    ptr = recvBuff;
 
    if (strncmp(recvBuff, "220", 3) == 0) {
        sprintf(sendBuff, "USER %s\r\n", ftpConf->username);
        logWrite(socket, sendBuff);
    }
    logRead(socket, recvBuff);
    if (strncmp(recvBuff, "331", 3) == 0) {
        sprintf(sendBuff, "PASS %s\r\n", ftpConf->password);
        logWrite(socket, sendBuff);
    }
    

    logRead(socket, recvBuff);
    
    while (strncmp(recvBuff, "230-", 4) == 0){
        char line[BUFFER_SIZE];
        int s, j=0, end = 0;

        while ((s = substrafter(recvBuff, recvBuff, '\n', 1)) == 0){
            if (strncmp(recvBuff, "230 ", 4) == 0) {
                end = 1; 
                break;
            }
        }
        if (end) break;
        logRead(socket, recvBuff);
    }    

    if (strncmp(recvBuff, "230", 3) != 0) {
        //Authentifiaction failed
        pdie(2, NULL);
    }
}

int logRead(int socket, char recvBuff[]){
    int n;

    n = read(socket, recvBuff, BUFFER_SIZE-1);     

    if(n < 0) {
        pdie(7, NULL);
    }

    checkForErrorResponse(recvBuff);
    
    recvBuff[n] = 0;

    if (gArgs.log != NULL) {
        pthread_mutex_lock (&mutlog);
        fprintf(gArgs.log, "S->C: %s", recvBuff);
        pthread_mutex_unlock (&mutlog);
    }
    return n;
}

void logWrite(int socket, char sendBuff[]) {
    write(socket, sendBuff, strlen(sendBuff));

    if (gArgs.log != NULL) {
        fprintf(gArgs.log, "C->S: %s", sendBuff);
    }
}

int connectSocket(char hostname[], int port) {
    struct hostent *he;
    int socket_fd = 0;
    struct sockaddr_in server_address; 
    memset(&server_address, '0', sizeof(server_address)); 

    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        pdie(1, NULL);
    } 

    if ((he = gethostbyname(hostname)) == NULL) {
        pdie(1, NULL);
    }

    memcpy(&server_address.sin_addr, he->h_addr_list[0], he->h_length);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port); 

    if( connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
       pdie(1, NULL);
    }

    return socket_fd;
}

int substrafter(char *out, char haystack[], char needle, int nr) {
    int i, needleNr = 0;

    for (i = 0; i < strlen(haystack); ++i) {
        if(haystack[i] == needle) {
            needleNr++;
        }
        if (i == strlen(haystack)-1){
            out = "";
            return 1;
        }

        if (needleNr == nr){
            strcpy(out, &haystack[i+1]);
            return 0;
        }
    }
    out = "";
    return -1;
}

void retriveFile(struct ftpArgs_t *ftpConf, char sendBuff[], char recvBuff[], int control_socket){
    int filesocket = 0, filesize = 0, bytesToDownload = 0, received = 0, startPos = 0, n, i;
    int connect_socket;
    unsigned char fileRecv[1024];
    

    memset(fileRecv, 0, sizeof(fileRecv));

    sprintf(sendBuff, "SIZE %s\r\n", ftpConf->filename);
    logWrite(control_socket, sendBuff);
    logRead(control_socket, recvBuff);
    
    if (strncmp(recvBuff,"550", 3) == 0) {
        pdie(3, NULL);
    }
    substrafter(recvBuff, recvBuff, ' ', 1);

    filesize = atoi(recvBuff);
    bytesToDownload = (ftpConf->threadNr != gArgs.nthreads - 1) ? ceil(filesize/gArgs.nthreads) : filesize - ceil(filesize/gArgs.nthreads) *(gArgs.nthreads-1);
    
    startPos = ceil(filesize/gArgs.nthreads) * ftpConf->threadNr;


    if (gArgs.active) {
        char portStr[40];
        connect_socket = openServerSocket();

        /* Write portsting and recieve 200 Port successful */
        portString(portStr, connect_socket);
        logWrite(control_socket, portStr);
        logRead(control_socket, recvBuff);

        sprintf(sendBuff, "REST %d\r\n", startPos);
        logWrite(control_socket, sendBuff);   
        sprintf(sendBuff, "RETR %s\r\n",ftpConf->filename);
        logWrite(control_socket, sendBuff);

        filesocket = connectToMessageSocket(connect_socket);
        logRead(control_socket, recvBuff);
    
    } else {
        /* PASV mode */
        int port;
        strcpy(sendBuff, "PASV\r\n");
        logWrite(control_socket, sendBuff);
        logRead(control_socket, recvBuff);
    
        port = findPasvPort(recvBuff);
        filesocket = connectSocket(ftpConf->hostname, port);
        sprintf(sendBuff, "REST %d\r\n", startPos);
        logWrite(control_socket, sendBuff);   
        sprintf(sendBuff, "RETR %s\r\n",ftpConf->filename);
        logWrite(control_socket, sendBuff);
        logRead(control_socket, recvBuff);
    }

    while (received < bytesToDownload) {
        n = read(filesocket, fileRecv, BUFFER_SIZE-1);;
        
        pthread_mutex_lock (&mutfile);
        fseek(gArgs.file,startPos + received, SEEK_SET);
        fwrite(fileRecv, 1, n, gArgs.file);
        pthread_mutex_unlock (&mutfile);
        received += n;
    }

    logWrite(control_socket, "ABOR\r\n");
}


/* Sets ascii or binary mode */
void setType(int control_socket, char sendBuff[]){

    if (strcmp(gArgs.mode, "binary") == 0 ) {
        strcpy(sendBuff, "TYPE I\r\n");
    } else if (strcmp(gArgs.mode, "ASCII") == 0) {
        strcpy(sendBuff, "TYPE A\r\n");
    }
    else {
        pdie(7, "Do not recognize type");
    }

    logWrite(control_socket, sendBuff);
}
 
/* port string */
void portString(char out[], int connectSocket) {
    char hostn[400]; //placeholder for the hostname
    char tmp[5];
    struct hostent *hostIP; //placeholder for the IP address
    struct sockaddr_in tmpSock;
    int i, port;
    int tmpSockLen;

    /* get port nr */
    tmpSockLen = sizeof(tmpSock);
    getsockname(connectSocket, (struct sockaddr *) &tmpSock, &tmpSockLen);
    port = ntohs(tmpSock.sin_port);

    if(gethostname(hostn, sizeof(hostn)) == 0) {
        hostIP = gethostbyname(hostn); //the netdb.h function gethostbyname
        sprintf(hostn, "%s", inet_ntoa(*(struct in_addr *)hostIP->h_addr));
        
        for (i = 0; i < strlen(hostn); i++) {
            if (hostn[i] == '.')
                hostn[i] = ',';
        }
        sprintf(out, "PORT %s,%d,%d\r\n", hostn, port/256, port%256);
    }else {
        pdie(2, NULL);
    }
}


int openServerSocket() {
    int socket_fd;
    int message_socket;
    struct sockaddr_in server_address;
    
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    bzero ((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(0);

    if (bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address))){
        pdie(1, NULL);
    }
    return socket_fd;

}

int connectToMessageSocket(int connectSocket){
    int clientLen, message_socket;
    struct sockaddr_in client_address;   /* socket struct for client connection */

    listen(connectSocket, 5);
    clientLen = sizeof(client_address);
    if ((message_socket = accept(connectSocket, (struct sockaddr *) &client_address, &clientLen)) < 0){
        pdie(1, NULL);
    }

    return message_socket;

}

void checkForErrorResponse(char response[]) {
    int responseCode;
    responseCode = (((int)response[0])-48)*100 + (((int)response[1])-48)*10 + (((int)response[2])-48);


    switch (responseCode) {
        case 500:
            pdie(4, NULL);
            break;

        case 501: 
            pdie(4, NULL);
            break;

        // Command not implemented
        case 202: 
            pdie(5, NULL);
            break;

        case 502:
            pdie(5, NULL);
            break;

        case 503:
            pdie(4, response);
            break;

        case 504:
            pdie(5, NULL);
            break;

        case 421:
            pdie(1, response);
            break;

        case 425:
            pdie(1, NULL);
            break;

        case 530:
            pdie(2, NULL);
            break;

        case 332:
            pdie(2, NULL);
            break;

        case 550:
            pdie(3, NULL);
            break;

        case 451:
            pdie(7, response);
            break;

        case 551:
            pdie(3, NULL);
            break;

        case 553:
            pdie(3, response);
            break;

        case 552:
            pdie(3, response);
            break;

        case 450:
            pdie(3, response);
            break;
    }
}

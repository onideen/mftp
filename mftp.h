#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <string.h> /* memset */
#include <unistd.h> /* close */
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <regex.h> 
#include <netdb.h>

int connectSocket(int port);
void pdie(int exitCode);
void printGlobalArgs();
void ftpClient();
void authentificate(int socket, char recvBuff[], char sendBuff[]);
void logRead(int socket, char recvBuff[]);
void logWrite(int socket, char sendBuff[]);
int findPasvPort(char searchString[]);
void substrafter(char out[], char in[], char needle[], int nr);
int findBytes(char haystack[]);
void retriveFile(char sendBuff[], char recvBuff[], int control_socket);
void setType(int control_socket, char sendBuff[]);
void portString(char out[], int connectSocket);
int openServerSocket();
int connectToMessageSocket(int connectSocket);


struct globalArgs_t {
	char *downloadFile;	/* -f option */
	char *hostname;		/* -s option */
	int port;			/* -p option */
	char *username;		/* -n option */	
	char *password;		/* -P option */
	int active;			/* -a option */
	char *mode;			/* -m option */
	char *logfile;		/* -l option */
	FILE *log;
} gArgs;

static const char *optString = "hvaf:s:p:n:P:m:l:";

static const struct option longOpts[] = {
	{ "version", no_argument, NULL, 'v' },
	{ "help", no_argument, NULL, 'h' },
	{ "file", no_argument, NULL, 'f' },
	{ "server", no_argument, NULL, 's' },
	{ "port", no_argument, NULL, 'p' },
	{ "username", no_argument, NULL, 'n' },
	{ "password", no_argument, NULL, 'P' },
	{ "active", no_argument, NULL, 'a' },
	{ "mode", no_argument, NULL, 'm' },
	{ "log", no_argument, NULL, 'l' },
	{ NULL, no_argument, NULL, 0 }
};
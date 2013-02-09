#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <string.h> /* memset */
#include <unistd.h> /* close */
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <regex.h> 

int connectSocket(int port);
void pdie(int errorCode);
void printGlobalArgs();
void ftpClient();
int authentificate(int socket, char recvBuff[], char sendBuff[]);
void printread(int socket, char recvBuff[]);
int findPasvPort(char searchString[]);
void substrafter(char out[], char in[], char needle[], int nr);
void retriveFile();


struct globalArgs_t {
	char *downloadFile;	/* -f option */
	char *hostname;		/* -s option */
	int port;			/* -p option */
	char *username;		/* -n option */	
	char *password;		/* -P option */
	int active;			/* -a option */
	char *mode;			/* -m option */
	char *logfile;		/* -l option */
} gArgs;

static const char *optString = "hvaf:s:p:n:P:m:l";

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
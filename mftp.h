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
	char *swarmfile;
	int nthreads;
} gArgs;

struct ftpArgs_t {
	char *username;
	char *hostname;
	char *password;
	char *filename;
	int port;
	int threadNr;
};


static const char *optString = "hvaf:s:p:n:P:m:l:w:";

static const struct option longOpts[] = {
	{ "version", no_argument, NULL, 'v' },
	{ "help", no_argument, NULL, 'h' },
	{ "file", required_argument, NULL, 'f' },
	{ "server", required_argument, NULL, 's' },
	{ "port", required_argument, NULL, 'p' },
	{ "username", required_argument, NULL, 'n' },
	{ "password", required_argument, NULL, 'P' },
	{ "active", no_argument, NULL, 'a' },
	{ "mode", required_argument, NULL, 'm' },
	{ "log", required_argument, NULL, 'l' },
	{ "swarm-config-file", required_argument, NULL, 'w' },
	{ NULL, no_argument, NULL, 0 }
};

void *ftpClient(struct ftpArgs_t *ftpConf);
int connectSocket(char hostname[], int port);
void pdie(int exitCode);
void printGlobalArgs();
void authentificate(struct ftpArgs_t *ftpConf, int socket, char recvBuff[], char sendBuff[]);
void logRead(int socket, char recvBuff[]);
void logWrite(int socket, char sendBuff[]);
int findPasvPort(char searchString[]);
void substrafter(char out[], char in[], char needle[], int nr);
int findBytes(char haystack[]);
void retriveFile(struct ftpArgs_t *ftpConf, char sendBuff[], char recvBuff[], int control_socket);
void setType(int control_socket, char sendBuff[]);
void portString(char out[], int connectSocket);
int openServerSocket();
int connectToMessageSocket(int connectSocket);
void parseSwarmConf(char out[], char opt, char line[]);

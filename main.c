#include "mftp.h" 

void printUsage();
void printVersion();

/* NOT DONE */
void printUsage() {
	printf("This is the help file\n");
	exit(0);
}

/* NOT DONE */
void printVersion() {
	printf("mftp 0.1\nAuthor: Vegar Engen");
	exit(0);
}
 
/* prints the right exit message to stderr and exits */
void pdie(int exitCode){
	char exitMessage[50];
	switch (exitCode) {
		case 0:
			strcpy(exitMessage, "Operation successfully completed");
			break;

		case 1:
			strcpy(exitMessage, "Can't connect to server");
			break;		

		case 2:
			strcpy(exitMessage, "Authentification failed");
			break;

		case 3:
			strcpy(exitMessage, "File not found");
			break;
			
		case 4:
			strcpy(exitMessage, "Syntax error in client request");
			break;
			
		case 5:
			strcpy(exitMessage, "Command not implemented by server");
			break;
			
		case 6: 
			strcpy(exitMessage, "Operation not allowed by server");
			break;
			
		case 7:
			strcpy(exitMessage, "Generic Error");
			break;
			
		default:
			/* Should never get here */
			strcpy(exitMessage, "ERROR");
			break;
	}

	fprintf(stderr, "%s\n", exitMessage);
	exit(exitCode);

}



int main(int argc, char **argv) {
	int opt = 0;
	int longIndex = 0;
	
	if (argc == 1) {
		printUsage();
	}

	/* Initialize gArgs before with default values */
	gArgs.downloadFile = NULL;
	gArgs.hostname = NULL;
	gArgs.port = 21;
	gArgs.username = "anonymous";
	gArgs.password = "user@localhost.localnet";
	gArgs.active = 0; /* False */
	gArgs.mode = "binary";
	gArgs.logfile = NULL;
	

	opt = getopt_long( argc, argv, optString, longOpts, &longIndex );

	while( opt != -1 ) {
		switch( opt ){
			case 'f':
				gArgs.downloadFile = optarg;
				break;

			case 's':
				gArgs.hostname = optarg;
				break;

			case 'p':
				gArgs.port = atoi(optarg);
				break;

			case 'n':
				gArgs.username = optarg;
				break;

			case 'a':
				gArgs.active = 1;	/* true */
				break;

			case 'm': 
				gArgs.mode = optarg;
				break;

			case 'l':
				gArgs.logfile = optarg;
				break;

			case 'h':
				printUsage();
				break;

			case 'v':
				printVersion();
				break;

			default:
				/* Will never get here */
				break;

		}
		
		opt = getopt_long( argc, argv, optString, longOpts, &longIndex );

	}


//	printGlobalArgs();
	
	ftpClient();

}


void printGlobalArgs() {
	printf("Download File: %s\nHostname: %s\nPort: %d\nUsername: %s\nPassword: %s\nActive: %d\nMode: %s\nLogfile: %s\n", gArgs.downloadFile, gArgs.hostname, gArgs.port, gArgs.username, gArgs.password, gArgs.active, gArgs.mode, gArgs.logfile);

}


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
			sprintf(exitMessage, "Exit(%d): Operation successfully completed", exitCode);;
			break;

		case 1:
			sprintf(exitMessage, "Exit(%d): Can't connect to server", exitCode);
			break;		

		case 2:
			sprintf(exitMessage, "Exit(%d):Authentification failed", exitCode);
			break;

		case 3:
			sprintf(exitMessage, "Exit(%d): File not found", exitCode);
			break;
			
		case 4:
			sprintf(exitMessage, "Exit(%d): Syntax error in client request", exitCode);
			break;
			
		case 5:
			sprintf(exitMessage, "Exit(%d): Command not implemented by server", exitCode);
			break;
			
		case 6: 
			sprintf(exitMessage, "Exit(%d): Operation not allowed by server", exitCode);
			break;
			
		case 7:
			sprintf(exitMessage, "Exit(%d): Generic Error", exitCode);
			break;
			
		default:
			/* Should never get here */
			strcpy(exitMessage, "ERROR");
			break;
	} 
	if (gArgs.logfile != NULL){
		fwrite(exitMessage, 1, strlen(exitMessage), gArgs.log);
		fclose(gArgs.log);
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
				gArgs.log = fopen(gArgs.logfile, "w");
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

	pdie(0);
}


void printGlobalArgs() {
	printf("Download File: %s\nHostname: %s\nPort: %d\nUsername: %s\nPassword: %s\nActive: %d\nMode: %s\nLogfile: %s\n", gArgs.downloadFile, gArgs.hostname, gArgs.port, gArgs.username, gArgs.password, gArgs.active, gArgs.mode, gArgs.logfile);

}


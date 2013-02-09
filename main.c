#include "mftp.h" 

void printUsage();
void printVersion();

/* NOT DONE */
void printUsage() {
	printf("This is the help file\n");
}

/* NOT DONE */
void printVersion() {
	printf("This is version 0.01\n");
}
 
/* NOT DONE - print right errormessage */
void pdie(int errorCode){
	printf("Error %d\n", errorCode);
	exit(errorCode);
}


int main(int argc, char **argv) {
	int opt = 0;
	int longIndex = 0;
	
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
				exit(0);
				break;

			case 'v':
				printVersion();
				exit(0);
				break;

			default:
				/* Will never get here */
				break;

		}
		
		opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
		
	}

	printGlobalArgs();
	
	ftpClient();

}


void printGlobalArgs() {
	printf("Download File: %s\nHostname: %s\nPort: %d\nUsername: %s\nPassword: %s\nActive: %d\nMode: %s\nLogfile: %s\n", gArgs.downloadFile, gArgs.hostname, gArgs.port, gArgs.username, gArgs.password, gArgs.active, gArgs.mode, gArgs.logfile);

}




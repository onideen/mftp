#include "mftp.h" 

void createFileForDownload(FILE *fpt, char *filename);
int linesInFile(FILE *f);

void printUsage(FILE *out) {
	fprintf(out, "Proper usage is: ./mftp [OPTIONS] \n");
	fprintf(out, "Option list: \n");
	fprintf(out, " -h, --help\t\t\tDisplays this helpfile\n");
	fprintf(out, " -v, --version\t\t\tPrints the name of the application and author\n");
	fprintf(out, " -f, --file\tfile\t\tSpecifies the file to download\n");
	fprintf(out, " -s, --server\thostname\tSpecifies the server to download file from\n");
	fprintf(out, " -p, --port\tport\t\tSpecifies the port to be used when contacting the server.\n");
	fprintf(out, " -n, --user\tuser\t\tUses the username user when contacting the server\n");
	fprintf(out, " -P, --password\tpassword\tUses the password password when\n\t\t\t\t logging into the FTP server\n");
	fprintf(out, " -a, --active\t\t\tForces active behavior\n");
	fprintf(out, " -m, --mode\t\t\tSpecifies the mode to be\n\t\t\t\t used for the transfer (ASCII or binary)\n");
	fprintf(out, " -l, --log\tlogfile\t\tLogs all the FTP commands exchanged with\n\t\t\t\t the server and the corresponding replies to file logfile. \n");
	fprintf(out, " -w, --swarm\tconfig-file\tSpecifies the login, password, hostname\n\t\t\t\t and absolute path to the file to use in the\n\t\t\t\t multitheaded swarm mode\n");
	exit(0);
}

void printVersion() {
	printf("mftp 0.1\nAuthor: Vegar Engen");
	exit(0);
}
 
/* prints the right exit message to stderr and exits */
void pdie(int exitCode, char *errorMessage){
	char exitMessage[50];

	if (errorMessage != NULL) {
		fprintf(stderr, errorMessage);
		
		if (gArgs.log != NULL) {
			fprintf(gArgs.log, errorMessage);
		}
	}

	switch (exitCode) {
		case 0:
			sprintf(exitMessage, "Exit(%d): Operation successfully completed", exitCode);
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
	if (gArgs.log != NULL){
		fprintf(gArgs.log, exitMessage);
		fclose(gArgs.log);
	}

	if (exitCode != 0) {
		unlink(gArgs.filename);
	}

	
	fprintf(stderr, "%s\n", exitMessage);
	
	

	exit(exitCode);

}



int main(int argc, char **argv) {
	int opt = 0, j;
	int longIndex = 0;
	struct ftpArgs_t *hosts;
	char tmp[100];

	if (argc == 1) {
		printUsage(stderr);
	}

	/* Initialize gArgs before with default values */
	gArgs.filename = NULL;
	gArgs.hostname = NULL;
	gArgs.port = 21;
	gArgs.username = "anonymous";
	gArgs.password = "user@localhost.localnet";
	gArgs.active = 0; /* False */
	gArgs.mode = "binary";
	gArgs.logfile = NULL;
	gArgs.nthreads = 1;

	opt = getopt_long( argc, argv, optString, longOpts, &longIndex );

	while( opt != -1 ) {
		switch( opt ){
			case 'f':
				gArgs.filename = optarg;
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
				if (strcmp(optarg, "-") == 0) {
					gArgs.log = stdout;
					break;
				} 
				gArgs.logfile = optarg;

				gArgs.log = fopen(gArgs.logfile, "w");
				break;

			case 'h':
				printUsage(stdout);
				break;

			case 'v':
				printVersion();
				break;

			case 'w':
				gArgs.swarmfile = optarg;
				break;

			case '?':
				printUsage(stderr);
				break;

			default:
				/* Will never get here */
				break;

		}

		opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
	}


	if (gArgs.swarmfile == NULL) {
		hosts = malloc(sizeof(struct ftpArgs_t));
		hosts[0].hostname = gArgs.hostname;
		hosts[0].port = gArgs.port;
		hosts[0].username = gArgs.username;
		hosts[0].password = gArgs.password;
		hosts[0].threadNr = 0;
		hosts[0].filename = gArgs.filename;
	} else {
		FILE *file = fopen(gArgs.swarmfile, "r");
		int lines = 0, i;

		lines = linesInFile(file);
		gArgs.nthreads = lines;
		
		hosts = malloc(sizeof(struct ftpArgs_t) * lines);

		for (i = 0; i < lines; i++) {
			char tmp[200];
			char line[200];

			fgets(line, sizeof(line),file);

			if (i == lines-1)
				strcat(line, "\n");

			if (strlen(line) < 5)
				continue;
						
			parseSwarmConf(tmp, 'u', line);
			hosts[i].username = malloc(sizeof(char) * strlen(tmp));
			strcpy(hosts[i].username, tmp);
			
			parseSwarmConf(tmp, 'p', line);
			hosts[i].password = malloc(sizeof(char) * strlen(tmp));
			strcpy(hosts[i].password, tmp);
			
			parseSwarmConf(tmp, 'h', line);
			hosts[i].hostname = malloc(sizeof(char) * strlen(tmp));
			strcpy(hosts[i].hostname, tmp);
			
			parseSwarmConf(tmp, 'f', line);
			hosts[i].filename = malloc(sizeof(char) * strlen(tmp));
			strcpy(hosts[i].filename, tmp);
			


			hosts[i].port = 21;
			hosts[i].threadNr = i;
			
		}
		gArgs.filename = hosts[0].filename;
	}

	// missing arguments ????

	while (substrafter(tmp, gArgs.filename, '/', 1) == 0) {
		strcpy(gArgs.filename, tmp);
	}
	if (substrafter(tmp, gArgs.filename, '\n', 1) == 1) {
		gArgs.filename[strlen(gArgs.filename)-1] = 0;
	}

	gArgs.file = fopen(gArgs.filename, "w");

	threads = malloc(sizeof (pthread_t)*gArgs.nthreads);
	pthread_mutex_init(&mutfile, NULL);
	pthread_mutex_init(&mutlog, NULL);

	for (j = 0; j < gArgs.nthreads; j++) {
		pthread_create(&threads[j], NULL, ftpClient, &hosts[j]);
	}

	pthread_exit(NULL);

	pdie(0, NULL);
}


void parseSwarmConf(char out[], char opt, char line[]) {
	char tmp[strlen(line)];
	char b[400];
	char e[400];

	switch (opt){
		case 'u': //get username
			substrafter(b, line, '/', 2);
			substrafter(e, b, ':', 1);		
			break;
		case 'p': //get password
			substrafter(b, line, ':', 2);
			substrafter(e, b, '@', 1);		
			break;
		case 'h': //get hostname
			substrafter(b, line, '@', 1);
			substrafter(e, b, '/', 1);
			break;
		case 'f':
			substrafter(b, line, '/', 3);
			substrafter(e, b, '\n', 1);
			break; 
	}

	b[strlen(b)-strlen(e)-1] = 0;
	
	sprintf(out, "%s", &b[0]);
	
}

void printGlobalArgs() {
	printf("Download File: %s\nHostname: %s\nPort: %d\nUsername: %s\nPassword: %s\nActive: %d\nMode: %s\nLogfile: %s\n", gArgs.filename, gArgs.hostname, gArgs.port, gArgs.username, gArgs.password, gArgs.active, gArgs.mode, gArgs.logfile);

}

int linesInFile(FILE *f){
	char c; 
	int lines = 0;

	//Count lines in file
	while((c = fgetc(f)) != EOF) {	
		if(c == '\n') {	
			lines++;
		}
	}

	if (c != '\n') lines++;
	rewind(f);

	return lines;
}

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
	int opt = 0, j;
	int longIndex = 0;
	struct ftpArgs_t *hosts;
	pthread_t *threads;


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
	gArgs.nthreads = 1;
	

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

			case 'w':
				gArgs.swarmfile = optarg;
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
	
	} else {
		FILE *file = fopen(gArgs.swarmfile, "r");
		int lines = 0, i;
		char c; 

		//Count lines in file
		while((c = fgetc(file)) != EOF) {	
			if(c == '\n') {	
				lines++;
			}
		}

		if (c != '\n') lines++;
		fclose(file);

		gArgs.nthreads = lines;
		
		file = fopen(gArgs.swarmfile, "r");
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
			hosts[i].hostname = malloc(sizeof(char) * strlen(tmp));
			strcpy(hosts[i].hostname, tmp);
			


			hosts[i].port = 21;
			hosts[i].threadNr = i;
			
			printf("username: %s\nhostname: %s\npassword: %s\nport: %d\nthreadNr: %d\n", hosts[i].username, hosts[i].hostname, hosts[i].password, hosts[i].port, hosts[i].threadNr);
		}
	}
	threads = malloc(sizeof (pthread_t)*gArgs.nthreads);
	
	exit(0);

	for (j = 0; j < gArgs.nthreads; j++) {
		pthread_t_create(&threads[j], NULL, ftpClient, &hosts[j]);
	}


	pdie(0);
}


void parseSwarmConf(char out[], char opt, char line[]) {
	char tmp[strlen(line)];
	char b[400];
	char e[400];

	switch (opt){
		case 'u': //get username
			substrafter(b, line, "/", 2);
			substrafter(e, b, ":", 1);		
			break;
		case 'p': //get password
			substrafter(b, line, ":", 2);
			substrafter(e, b, "@", 1);		
			break;
		case 'h': //get hostname
			substrafter(b, line, "@", 1);
			substrafter(e, b, "/", 1);
			break;
		case 'f':
			substrafter(b, line, "/", 3);
			substrafter(e, b, "\n", 1);
			break; 
	}

	b[strlen(b)-strlen(e)-1] = 0;
	
	sprintf(out, "%s", &b[0]);
	
}

void printGlobalArgs() {
	printf("Download File: %s\nHostname: %s\nPort: %d\nUsername: %s\nPassword: %s\nActive: %d\nMode: %s\nLogfile: %s\n", gArgs.downloadFile, gArgs.hostname, gArgs.port, gArgs.username, gArgs.password, gArgs.active, gArgs.mode, gArgs.logfile);

}

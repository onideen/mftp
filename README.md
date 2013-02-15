Multithreaded File Transfer Protocol
====================================

The Mulitthreaded File Transfer Protocol (mftp) is a swarming ftp client written in c in the class CS176b at UCSB. It supports download a file specified in cli. The way the mftp client differs from a standard ftp client is that it supports swarming downloads, which means it can download a file simultaneously from multiple ftp-servers. 


Compile
-------
To compile the project run make from folder root


Usage
-----
	
	Proper usage is: ./mftp [OPTIONS]

Option list: 
	
	-h, --help					Displays this helpfile
 	-v, --version				Prints the name of the application and author
	-f, --file		file		Specifies the file to download
	-s, --server	hostname	Specifies the server to download file from
	-p, --port		port		Specifies the port to be used when contacting the server.
	-n, --user		user		Uses the username user when contacting the server
	-P, --password	password	Uses the password password when logging into the FTP server
	-a, --active				Forces active behavior
	-m, --mode		mode		Specifies the mode to be used for the transfer (ASCII or binary)
	-l, --log		logfile		Logs all the FTP commands exchanged with the server and the corresponding replies to file logfile. 
 	-w, --swarm		config-file	Specifies the login, password, hostname and absolute path to the file to use in the multitheaded swarm mode
 
Default values:

	port = 21
	user = anonymous
	mode = binary
	password = user@localhost.localnet

**Hostname** works for both dns and ip-addresses.  
**Mode** only *binary* or *ASCII* is accepted as options.
**Swarm-config** specifies a file where login info is stored in this format *ftp://username:password@server/file*

Sample swarm-config-file:

	ftp://anonymous:shabadabadoo@location.dnsdynamic.com/10.MB
	ftp://anonymous:securepassword@128.111.68.213/10.MB

*--help* prints usage to stdout  
When calling the program it is required to either specify **hostname** AND **file** OR a **swarm-config-file**.



Error Handling
--------------
*	If the cli command is missing required arguments the mftp-client will exit with code 0 and print usage in stderr.
*	If there is something wrong with the swarming file the client dies a horrible death.
*	The mftp client checks for error responses each time it gets a response from the server. If an error response is found it exits with the appropriate exit code and write something useful to stderr.  
*	If **one** thread fails, the whole program fails!
*	If *swarm-config-file* does not exists the program exits
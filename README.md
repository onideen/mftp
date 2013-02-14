mftp
====

mftp is a swarming ftp client written in c in the class CS176b at UCSB. It supports download a file specified in cli. This client differs from a standard ftp client because it supports swarming downloads, which means it can download a file simultaneously from multiple ftp-servers. 




Usage
-----
	
	Proper usage is: ./mftp [OPTIONS]

Option list: 
	
	-h, --help					Displays this helpfile
 	-v, --version				Prints the name of the application and author
	-f, --file		*file*		Specifies the file to download
	-s, --server	*hostname*	Specifies the server to download file from
	-p, --port		*port*		Specifies the port to be used when contacting the server.
	-n, --user		*user*		Uses the username user when contacting the server
	-P, --password	*password*	Uses the password password when logging into the FTP server
	-a, --active				Forces active behavior
	-m, --mode		*mode*		Specifies the mode to be used for the transfer (ASCII or binary)
	-l, --log		*logfile*	Logs all the FTP commands exchanged with the server and the corresponding replies to file logfile. 
 	-w, --swarm		*config-file*	Specifies the login, password, hostname and absolute path to the file to use in the multitheaded swarm mode
 
Default values:

	*port* = 21
	*user* = anonymous
	*password* = user@localhost.localnet


Hostname works for both dns and ip-addresses. 



Error Handling
--------------
The mftp client checks for error responses each time it gets a response from the server. If an error response is found it exits with the appropriate exit code and write something useful to stderr.  





-h -> prints usage to stdout
on error print to stderr

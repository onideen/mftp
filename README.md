mftp
====

swarming ftp client written for cs176B 



USAGE
=====
Proper usage is: ./mftp [OPTIONS]\n
Option list: 

-h, --help			Displays this helpfile
 
 -v, --version			Prints the name of the application and author
 -f, --file	file		Specifies the file to download
 -s, --server	hostname	Specifies the server to download file from
 -p, --port	port		Specifies the port to be used when contacting the server.
 -n, --user	user		Uses the username user when contacting the server
 -P, --password	password	Uses the password password when
				 logging into the FTP server
 -a, --active			Forces active behavior
 -m, --mode			Specifies the mode to be
				 used for the transfer (ASCII or binary)
 -l, --log	logfile		Logs all the FTP commands exchanged with
				 the server and the corresponding replies to file logfile. 
 -w, --swarm	config-file	Specifies the login, password, hostname
				 and absolute path to the file to use in the
				 multitheaded swarm mode









-h -> prints usage to stdout
on error print to stderr

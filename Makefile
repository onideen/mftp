mftp:	main.c mftp.c	
	gcc main.c mftp.c -o mftp -lpthread

clean:
	rm -rf *o mftp
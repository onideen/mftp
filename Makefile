mftp:	main.c mftp.c	
	gcc main.c mftp.c -o mftp -lpthread -lm

clean:
	rm -rf *o mftp
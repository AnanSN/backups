all:
	gcc server.c -o server -Wall -lsqlite3
	gcc client.c -o client -Wall

.PHONY : clean runs runc
	
clean:
	@rm -f a.out server client

runs:
	./server 192.168.1.16 8000

runc:
	./client 192.168.1.16 8000



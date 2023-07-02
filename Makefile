CFLAGS+= -mssse3 -g -O1 -Wall -Werror -std=c99

compile:
	$(CC) $(CFLAGS) main.c -lm -o main

test:
	$(CC) $(CFLAGS) test.c -lm -o test 

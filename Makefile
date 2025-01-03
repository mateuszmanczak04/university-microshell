microshell: microshell.c
	gcc -std=c17 -Wall -ansi -o microshell microshell.c

clean:
	rm -f *.o microshell
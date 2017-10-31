all : master slave

clean:
	rm master slave

master: master.c
	gcc -o master master.c -lm -lpvm3

slave: slave.c
	gcc  -o slave slave.c -lm -lpvm3


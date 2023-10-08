all: rainbow_create rainbow_attack hash_many

rainbow_attack : rainbow_attack.o 
	gcc -o rainbow_attack rainbow_attack.o

rainbow_attack.o : rainbow_create.c
	gcc -c rainbow_attack.c

rainbow_create : rainbow_create.o
	gcc -o rainbow_create rainbow_create.o -lm -lssl -lcrypto -g

rainbow_create.o : rainbow_create.c
	gcc -c rainbow_create.c

test_create :
	./rainbow_create res/test1.txt res/test2.txt res/test3.txt

hash_many : hash_many.o
	gcc -o hash_many hash_many.o

hash_many.o : hash_many.c
	gcc -c hash_many.c

test : test.o
	gcc -o test test.o -lssl -lcrypto

test.o : test.c
	gcc -c test.c

clean : 
	rm -f *.o rainbow_attack rainbow_create hash_many test

.PHONY: all clean
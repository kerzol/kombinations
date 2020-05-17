loopless: loopless.c
	gcc -W -Wall -Wno-comment -O3 loopless.c -o loopless

clean:
	rm loopless

all: maxTweeter.x

maxTweeter.x: maxTweeter.o
	gcc -Wall -Werror maxTweeter.o -o maxTweeter.x

maxTweeter.o: maxTweeter.c
	gcc -Wall -Werror -c maxTweeter.c

clean:
	rm -rf maxTweeter.o maxTweeter.x
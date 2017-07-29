FILENAME = mixItUp.c error.c
OUTPUT = mixItUp

all:
	gcc -Wall -Wpedantic -std=c99 -pthread ${FILENAME} -o ${OUTPUT}
clean:
	rm ${OUTPUT}

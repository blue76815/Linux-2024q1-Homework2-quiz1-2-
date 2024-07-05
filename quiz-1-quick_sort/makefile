SHELL = /bin/bash
CC = gcc
CFLAGS = -g -pthread
SRC = $(wildcard *.c)
EXE = $(patsubst %.c, %, $(SRC))

SRC2 = $(wildcard *.csv)
SRC3 = $(wildcard *.png)

all: ${EXE}

%:	%.c
	${CC} ${CFLAGS} $@.c -o $@


plot:
	sudo taskset -c 0 ./main > data.csv 
	gnuplot plot_qsort.gp
	mv data.csv  2000_data.csv 
	mv qsort.png 2000_qsort.png

clean:
	rm ${EXE}
	rm ${SRC2} 
	rm ${SRC3}

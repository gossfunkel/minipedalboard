CFLAGS := -std=c99 -Iincludes -Wall -msse2 -mavx2 -lm
CC := gcc

all: miniaudio compress distort repitch flanger

miniaudio:
	$(CC) -c miniaudio.c $(CFLAGS) -o miniaudio.o

compress: miniaudio
	$(CC) -c ma_comp.c $(CFLAGS) -o ma_comp.o
	$(CC) test_comp.c miniaudio.o ma_comp.o $(CFLAGS) -o test_comp.exe

distort: miniaudio
	$(CC) -c ma_distort.c $(CFLAGS) -o ma_distort.o
	$(CC) test_distort.c miniaudio.o ma_distort.o $(CFLAGS) -o test_distort.exe

repitch: miniaudio
	$(CC) -c ma_repitch.c $(CFLAGS) -o ma_repitch.o
	$(CC) test_repitch.c miniaudio.o ma_repitch.o $(CFLAGS) -o test_repitch.exe

flanger: miniaudio
	$(CC) -c ma_flanger.c $(CFLAGS) -o ma_flanger.o
	$(CC) test_flanger.c miniaudio.o ma_flanger.o $(CFLAGS) -o test_flanger.exe

clean:
	rm miniaudio.o
	rm ma_comp.o
	rm ma_distort.o
	rm ma_repitch.o
	rm test_comp.exe
	rm test_distort.exe
	rm test_repitch.exe
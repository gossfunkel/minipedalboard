CFLAGS := -std=c99 -Iincludes -Wall -msse2 -mavx2 -lm
CC := gcc

miniaudio:
	$(CC) -c miniaudio.c $(CFLAGS) -o miniaudio.o

comp: miniaudio
	$(CC) -c ma_comp.c $(CFLAGS) -o ma_comp.o
	$(CC) test_comp.c miniaudio.o ma_comp.o $(CFLAGS) -o test_comp.exe

distort: miniaudio
	$(CC) -c p3d_distort.c $(CFLAGS) -o p3d_distort.o
	$(CC) test_distort.c miniaudio.o p3d_distort.o $(CFLAGS) -o test_distort.exe

clean:
	rm miniaudio.o
	rm ma_comp.o
	rm test_comp.exe
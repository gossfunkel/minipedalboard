CFLAGS := -std=c99 -Iincludes -Wall -msse2 -mavx2 -lm
CC := gcc

miniaudio:
	$(CC) -c miniaudio.c $(CFLAGS) -o miniaudio.o

comp: miniaudio
	$(CC) -c p3d_comp.c $(CFLAGS) -o p3d_comp.o
	$(CC) test_comp.c miniaudio.o p3d_comp.o $(CFLAGS) -o test_comp.exe

distort: miniaudio
	$(CC) -c p3d_distort.c $(CFLAGS) -o p3d_distort.o
	$(CC) test_distort.c miniaudio.o p3d_distort.o $(CFLAGS) -o test_distort.exe

repitch: miniaudio
	$(CC) -c p3d_repitch.c $(CFLAGS) -o p3d_repitch.o
	$(CC) test_repitch.c miniaudio.o p3d_repitch.o $(CFLAGS) -o test_repitch.exe

clean:
	rm miniaudio.o
	rm p3d_comp.o
	rm p3d_distort.o
	rm p3d_repitch.o
	rm test_comp.exe
	rm test_distort.exe
	rm test_repitch.exe
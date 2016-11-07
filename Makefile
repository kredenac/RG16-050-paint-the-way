main: main.c player.c keyboard.c collision.c draw.c miscfunc.c
	gcc main.c player.c keyboard.c miscfunc.c draw.c collision.c -o main -lGL -lGLU -lglut -lm -g -Wall

clean:
	rm main
	rm *.o
	rm *~

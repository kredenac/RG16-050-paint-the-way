main: main.c player.c keyboard.c
	gcc main.c player.c keyboard.c miscfunc.c -o main -lGL -lGLU -lglut -lm -g -Wall

clean:
	rm main
	rm *.o
	rm *~

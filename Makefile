main: main.c
	gcc main.c -o main -lGL -lGLU -lglut -lm -g -Wall

clean:
	rm main
	rm *.o
	rm *~

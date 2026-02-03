main: ./src/main.c ./src/point.c ./src/array.c
	mkdir -p bin
	cc -Wall -Wextra \
	-I./include \
	-L./lib \
	-Wl,-rpath,'$(PWD)/lib' \
	./src/main.c ./src/point.c ./src/array.c \
	-lraylib -lm -ldl -lpthread -lGL -lX11 \
	-o ./bin/main

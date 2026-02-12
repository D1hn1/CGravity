SRCS := $(wildcard ./src/*.c)

bin/main: $(SRCS)
	mkdir -p bin
	cc -Wall -Wextra \
	-I./include \
	-L./lib \
	-Wl,-rpath,'$(PWD)/lib' \
	$(SRCS) \
	-lraylib -lm -ldl -lpthread -lGL -lX11 \
	-o ./bin/main

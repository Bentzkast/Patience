debug-build:
	g++ -g -Wall -std=c++11 -fno-exceptions \
	./src/*.cpp \
	-o game \
	-Iexternal/GLAD/include external/GLAD/src/glad.c \
	-Iexternal/stbimage external/stbimage/stb_image.cpp \
	-lGL \
	-lX11 \
	-ldl \
	-lpthread \
	-lXrandr \
	-lSDL2;

run:
	./game

clean:
	rm ./game;
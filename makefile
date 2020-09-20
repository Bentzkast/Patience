debug-build-linux:
	g++ -g -Wall -std=c++11 -fno-exceptions \
	./src/*.cpp \
	-o target/linux/game \
	-Iexternal/GLAD/include external/GLAD/src/glad.c \
	-Iexternal/stbimage external/stbimage/stb_image.cpp \
	-Iinclude \
	-lGL \
	-lX11 \
	-ldl \
	-lpthread \
	-lXrandr \
	-lfreetype \
	-lSDL2;

release-linux:
	g++ -O3 -Wall -std=c++11 -fno-exceptions \
	./src/*.cpp \
	-o target/linux/game \
	-Iexternal/GLAD/include external/GLAD/src/glad.c \
	-Iexternal/stbimage external/stbimage/stb_image.cpp \
	-Iinclude \
	-lGL \
	-lX11 \
	-ldl \
	-lpthread \
	-lXrandr \
	-lfreetype \
	-lSDL2;

debug-build-window:
	g++ -g -Wall -std=c++11 -fno-exceptions -static-libgcc -static-libstdc++ -lpthread \
	./src/*.cpp \
	-o target/window/game \
	-Iexternal/GLAD/include external/GLAD/src/glad.c \
	-Iexternal/stbimage external/stbimage/stb_image.cpp \
	-L"external/freetype" \
	-Iinclude \
	-lmingw32 \
	-lopengl32 \
	-lSDL2main \
	-lfreetype \
	-lSDL2;

run:
	./target/linux/game

run-window:
	target/window/game.exe

clean-window:
	rm target/window/game.exe

clean:
	rm ./target/linux/game;
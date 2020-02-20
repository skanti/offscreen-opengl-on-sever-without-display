HOME_DIR:= $(realpath ./)
all : main

DEFINES:=-DHOME_DIR=\""$(HOME_DIR)/\""
main : main.cpp
	c++ -I./base -L/usr/lib/nvidia-396/ -std=c++14 -Wall $(DEFINES) -O3 -o $@ $< -lglfw -lGLEW -lGL -lX11

clean:
	rm main


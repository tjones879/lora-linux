CFLAGS = --std=c++17 -Weverything -Wno-c++98-compat -Wno-global-constructors \
	 -Wno-exit-time-destructors -o main `pkg-config --cflags libsodium` -I.
LDFLAGS = `pkg-config --libs libsodium`
CXX = clang++
SOURCES = src/init.cpp src/main.cpp src/utils.cpp

all: src/main.cpp
	@$(CXX) $(CFLAGS) $(LDFLAGS) $(SOURCES)

run: all
	@./main

.PHONY = all test

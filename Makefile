CXX = clang++
CFLAGS = --std=c++17 -Weverything -Wno-c++98-compat -Wno-global-constructors \
	 -Wno-exit-time-destructors -o main  -I.
PACKAGES = `pkg-config --cflags libsodium sqlite3`
LDFLAGS = `pkg-config --libs libsodium sqlite3`
SOURCES = src/init.cpp src/main.cpp src/utils.cpp

all: src/main.cpp
	@$(CXX) $(CFLAGS) $(PACKAGES) $(LDFLAGS) $(SOURCES)

run: all
	@./main

.PHONY = all test

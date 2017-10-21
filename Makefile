CXX = clang++
CFLAGS = --std=c++17 -Weverything -Wno-c++98-compat -Wno-global-constructors \
	 -Wno-exit-time-destructors -Wno-zero-as-null-pointer-constant -o main  -I.
PACKAGES = `pkg-config --cflags libsodium sqlite3`
LDFLAGS = `pkg-config --libs libsodium sqlite3`
SOURCES = src/init.cpp src/contact.cpp src/db.cpp src/main.cpp src/utils.cpp

all: src/main.cpp
	$(CXX) $(PACKAGES) $(CFLAGS) $(LDFLAGS) $(SOURCES)

run: all
	@./main

.PHONY = all test

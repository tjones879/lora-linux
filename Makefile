CXX = clang++
CFLAGS = --std=c++17 -stdlib=libc++  -I.
WARNINGS = -Weverything -Wno-c++98-compat -Wno-global-constructors \
	 -Wno-exit-time-destructors -Wno-zero-as-null-pointer-constant \
	 -Wno-padded
PACKAGES = `pkg-config --cflags libsodium sqlite3`
LDFLAGS = `pkg-config --libs libsodium sqlite3` -l pthread
SOURCES = src/init.cpp src/db.cpp src/utils.cpp src/driver.cpp \
	  src/message.cpp src/packet.cpp
TESTS = test/main.cpp

main: src/main.cpp $(SOURCES)
	@$(CXX) -o main $(PACKAGES) $(WARNINGS) $(CFLAGS) $(LDFLAGS) src/main.cpp $(SOURCES)

run: main
	@./main

check: unittests
	@./unittests

unittests: main $(TESTS)
	$(CXX) -o unittests $(CFLAGS) -Ibandit/ $(LDFLAGS) $(SOURCES) $(TESTS)

.PHONY = run check

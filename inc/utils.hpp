/*
 * Utility functions shared by all the demo programs.
 */
#ifndef UTILS_H
#define UTILS_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>

#include "sodium_header.hpp"

#define MAX_INPUT_LEN 4096
namespace utils
{
/*
 * print_hex() is a wrapper around sodium_bin2hex() which allocates
 * temporary memory then immediately prints the result followed by \n
 */
int print_hex(const void *bin, const size_t bin_len, std::ostream &output);

void read_hex(std::vector<char> &hex, std::istream &input);

/*
 * Display a prompt for input by user. It will save the input into a buffer
 * of a specific size with room for the null terminator while removing
 * trailing newline characters.
 */
size_t prompt_input(const char *prompt, unsigned char *input, const size_t max_input_len, int variable_length);

/*
 * Display whether the function was sucessful or failed.
 */
void print_verification(int ret);

void init(void);
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
#endif /* UTILS_H */

/*
 * main.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: ezaxnyu
 */

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "InputBuffer.h"
#define EXIT_SUCCESS 1

using namespace std;

void print_prompt()
{
	cout << "db > ";
}

void read_input(InputBuffer* buffer)
{
	size_t bytes_read = getline
}

int main(int argc, char const *argv[]) {
  /* code */
    InputBuffer* input_buffer = new InputBuffer();
    while(true)
    {
        print_prompt();
        read_input(input_buffer);

        if(strcmp(input_buffer->buffer, ".exit"))
        {
        	exit(EXIT_SUCCESS);
        }
        else {
        	cout << "unrecognizd command " << input_buffer->buffer << endl;
        }
    }
    return 0;
}

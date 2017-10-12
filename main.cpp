/*
 * main.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: ezaxnyu
 */

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "InputBuffer.h"

#define EXIT_ERROR 0
#define EXIT_SUCCESS 1

using namespace std;

void print_prompt()
{
    cout << "db > ";
}

void read_input(InputBuffer* buffer)
{
    size_t bytes_read = getline(&(buffer->buffer), &(buffer->buffer_length), stdin);
    if(bytes_read < 0 )
    {
        cout << "error in reading input" << endl;
        exit(EXIT_ERROR);		
    }
    buffer->input_length = bytes_read-1;
    //replace the last char which is newline char
    buffer->buffer[bytes_read-1] = '\0';
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

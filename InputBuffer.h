/*
 * InputBuffer.h
 *
 *  Created on: Oct 12, 2017
 *      Author: ezaxnyu
 */

#ifndef INPUTBUFFER_H_
#define INPUTBUFFER_H_

class InputBuffer {
public:
	InputBuffer()
	{
		buffer = "";
		buffer_length = 0;
		input_length = 0;
	}
	virtual ~InputBuffer();

	char* buffer;
	size_t buffer_length;
	size_t input_length;
};



#endif /* INPUTBUFFER_H_ */

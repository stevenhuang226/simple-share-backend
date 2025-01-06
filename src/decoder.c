#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "../include/define.h"
#include "../include/function.h"

void decode_req(char *buffer)
{
	if (buffer == NULL) {
		return;
	}

	int buffer_len = strlen(buffer);
	for (int i = 2; i < buffer_len; i += 1) {
		if (buffer[i-2] == '%' && buffer[i-1] == '2' && buffer[i] == '0') {
			buffer[i-2] = ' ';
			memcpy(&buffer[i-1], &buffer[i+1], buffer_len - i);
			buffer_len -= 2; // after move the all string after the %20,
					 // the string total length will decrease 2
		}
	}
}

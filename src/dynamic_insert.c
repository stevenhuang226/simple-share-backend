#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "../config.h"
#include "../include/define.h"
#include "../include/function.h"
int8_t content_length_insert(char *buffer, int32_t buffer_size, int32_t content_long, int32_t insert_location)
{
	// check buffer
	if (buffer == NULL) {
		return ERROR;
	}
	// check buffer size
	int32_t buffer_len = strlen(buffer);
	if (buffer_size < buffer_len + 17 || content_long < 0) {
		return ERROR;
	}

	// translate int32_t to text number
	char content_str[16];
	int cnt_ptr = 15;
	do {
		content_str[cnt_ptr] = (content_long % 10) + '0';
		content_long /= 10;
		cnt_ptr -= 1;
	} while (content_long != 0);
	cnt_ptr += 1;

	// set cpy source and target
	int dest = insert_location + (16 - cnt_ptr);
	int src = insert_location;
	int len = buffer_len - insert_location + 1;

	// cpy the translate result
	memmove(&buffer[dest], &buffer[src], len);
	len = 16 - cnt_ptr;
	memcpy(&buffer[src], &content_str[cnt_ptr], len);
	return 0;
}
int8_t string_insert(char *buffer, int32_t buffer_size, const char *file_name, int32_t insert_location)
{
	// check buffer
	if (buffer == NULL) {
		return ERROR;
	}
	// check buffer size
	int buffer_len = strlen(buffer);
	int insert_len = strlen(file_name);
	if (buffer_size < buffer_len + 48) {
		return ERROR;
	}

	int dest = insert_location + insert_len;
	int src = insert_location;
	int len = buffer_len - insert_location + 1;
	memmove(&buffer[dest], &buffer[src], len);
	memcpy(&buffer[src], file_name, insert_len);

	return 0;
}

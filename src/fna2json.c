#include <stdint.h>
#include <string.h>

#include "../config.h"
#include "../include/define.h"

int8_t fna2json(char *buffer, int32_t buffer_size, char ***array, const int32_t *nums_files)
{
	if (buffer == NULL || *array == NULL || buffer_size < 32) {
		return FILE_NOT_EXISTS;
	}
	memcpy(buffer, "{\"file_array\":[", 15);
	int buff_ptr = 15;

	int insert_size = 0;

	for (int i = 0; i < *nums_files; i += 1) {
		insert_size = strlen((*array)[i]);
		// check buffer_overflow
		if (insert_size + buff_ptr + 32 >= buffer_size) {
			return BUFFER_OVERFLOW;
		}
		buffer[buff_ptr] = '"';
		buff_ptr += 1;
		memcpy(&buffer[buff_ptr], (*array)[i], insert_size);
		buff_ptr += insert_size;
		buffer[buff_ptr] = '"';
		buff_ptr += 1;
		buffer[buff_ptr] = ',';
		buff_ptr += 1;
	}
	memcpy(&buffer[buff_ptr-1], "]}", 2);
	return 0;
}

#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

#include "../config.h"
#include "../include/define.h"
#include "../include/http_header.h"

#define FORECAST_ADDITION 64

char *generate_header(struct header *header)
{
	char *buffer;
	buffer = malloc(HEADER_BUFFER_SIZE * sizeof(char));
	if (buffer == NULL) {
		return NULL;
	}

	ssize_t ptr = 0;

	if (header->req_stat[0] == '\0') {
		free(buffer);
		return NULL;
	}
	
	strncpy(&buffer[ptr], header->req_stat, HEADER_BUFFER_SIZE - ptr);
	ptr += strlen(header->req_stat);
	strncpy(&buffer[ptr], "\r\n", HEADER_BUFFER_SIZE - ptr);
	ptr += 2;

	if (header->content_type[0] != '\0') {
		strncpy(&buffer[ptr], "Content-Type: ", HEADER_BUFFER_SIZE - ptr);
		ptr += strlen("Content-Type: ");
		strncpy(&buffer[ptr], header->content_type, HEADER_BUFFER_SIZE - ptr);
		ptr += strlen(header->content_type);
		strncpy(&buffer[ptr], "\r\n", HEADER_BUFFER_SIZE - ptr);
		ptr += 2;
	}

	if (header->file_name[0] != '\0') {
		strncpy(&buffer[ptr], "Content-Disposition: attachment; filename=\"", HEADER_BUFFER_SIZE - ptr);
		ptr += strlen("Content-Disposition: attachment; filename=\"");
		strncpy(&buffer[ptr], header->file_name, HEADER_BUFFER_SIZE - ptr);
		ptr += strlen(header->file_name);

		strncpy(&buffer[ptr], "\"\r\n", HEADER_BUFFER_SIZE - ptr);
		ptr += 3;
	}

	if (header->content_length < 0 || HEADER_BUFFER_SIZE - ptr - FORECAST_ADDITION < 0) {
		free(buffer);
		return NULL;
	}

	strncpy(&buffer[ptr], "Content-Length: ", HEADER_BUFFER_SIZE - ptr);
	ptr += strlen("Content-Length: ");

	int length = header->content_length;

	char cnt_str[16];
	int cnt_ptr = 15;
	do {
		cnt_str[cnt_ptr] = (length % 10) + '0';
		length /= 10;
		cnt_ptr -= 1;
	} while(length != 0);
	cnt_ptr += 1;

	memcpy(&buffer[ptr], &cnt_str[cnt_ptr], 16-cnt_ptr);
	ptr += (16-cnt_ptr);

	strncpy(&buffer[ptr], "\r\n", HEADER_BUFFER_SIZE - ptr);
	ptr += 2;

	if (header->connection[0] != '\0') {
		strncpy(&buffer[ptr], header->connection, HEADER_BUFFER_SIZE - ptr);
		ptr += strlen(header->connection);
		strncpy(&buffer[ptr], "\r\n", HEADER_BUFFER_SIZE - ptr);
		ptr += 2;
	}

	strncpy(&buffer[ptr], "\r\n", HEADER_BUFFER_SIZE - ptr);
	ptr += 2;

	header->size = ptr;
	return buffer;
}

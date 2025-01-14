#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "../config.h"
#include "../include/define.h"
#include "../include/function.h"
#include "../include/request_types.h"

ssize_t content_length(const char *buffer, int32_t max_range, int32_t max_offset)
{
	// find the start of Content-Length
	char *target_begin = strstr(buffer, "Content-Length: ");
	if (target_begin == NULL) {
		return NOT_FOUND;
	}

	// set the offset to target_begin
	target_begin += strlen("Content-Length: ");

	// strtol will return all of number it can convert,
	// no needed to find the end of number
	char *target_end = NULL;
	ssize_t num = (ssize_t)strtol(target_begin, &target_end, 10);
	// target_end equal to target_begin => no number convert success,
	if (target_end == target_begin) {
		return NOT_FOUND;
	}

	return num;
}
int8_t req_type(const char *buffer)
{
	if (buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T') {
		return GET_REQ;
	}
	if (buffer[0] == 'P' && buffer[1] == 'U' && buffer[2] == 'T') {
		return PUT_REQ;
	}
	return ERROR;
}
int32_t get_handle(const char *req, char ***array, const int32_t *nums_files)
{
	char path[HDLE_PATH_LEN];

	int8_t found = 0;
	for (int i = 5; i < HDLE_PATH_LEN + 5; i += 1) {
		if (req[i] == ' ') {
			memcpy(path, &req[5], i-5);
			path[i-5] = '\0';
			found = 1;
			break;
		}
		// ".." in string is not allow
		if (req[i-1] == '.' && req[i] == '.') {
			return ERROR;
		}
	}
	// the loop cannot found path space (end)
	if (found == 0) {
		return ERROR;
	}
	// translate %20 to space(0x20)
	decode_req(path);
	// set html file name into path if path is empty and found != 0
	if (path[0] == '\0') {
		return HOME_REQ;
	}
	if (strcmp(path, FNA_API_NAME) == 0) {
		return FNA_API_REQ;
	}
	if (strcmp(path, JS_FILE_NAME) == 0) {
		return JS_REQ;
	}
	// check is path in file_name_array
	for (int i = 0; i < *nums_files; i += 1) {
		if (strcmp(path, (*array)[i]) == 0) {
			return (int32_t)i;
		}
	}
	return FILE_NOT_EXISTS;
}
int8_t put_handle(const char *req, const char *path2save)
{
	// create path buffer
	char path[PUT_PATH_LEN];
	// cpy the path2save into path
	strcpy(path, path2save);
	size_t path_ptr = strlen(path2save);

	// search end ( space 0x20 )
	int8_t found = 0;
	for (int i = 5; i < PUT_PATH_LEN + 5; i += 1) {
		if (req[i] == ' ') {
			memcpy(&path[path_ptr], &req[5], i-5);
			path[i - 5 + path_ptr] = '\0';
			found = 1;
			break;
		}
		if (req[i-1] == '.' && req[i] == '.') {
			return ERROR;
		}
	}
	// search content-length
	ssize_t content_size = content_length(req, CONTENT_FIND_RANGE, MAX_CNT_LEN_OFFSET);
	// if no found or content-length not exists
	if (found == 0 || content_size < 0) {
		return ERROR;
	}
	// set the beign of the data
	const char *body_begin = strstr(req, "\r\n\r\n");
	if (! body_begin) {
		return ERROR;
	}
	body_begin += 4; // add the offset
	// decode the url path to normal string
	decode_req(path);
	// open and set authorize to all usr
	int file_fd = open(path, O_CREAT | O_WRONLY | O_EXCL,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	
	if (file_fd < 0) {
		return ERROR;
	}
	// write the data into file
	ssize_t bytes_written = write(file_fd, body_begin, content_size);
	close(file_fd);
	// check the write success and wrote value
	if (bytes_written < 0 || bytes_written != content_size) {
		return ERROR;
	}

	return 0;
}

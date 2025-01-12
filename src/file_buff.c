#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "../include/define.h"
#include "../include/function.h"

int8_t file2buffer(char *buffer, const int32_t buffer_size, const char *path)
{
	// open and check
	int file_fd = open(path, O_RDONLY);
	if (file_fd < 0 && errno == ENOENT) {
		return FILE_NOT_EXISTS;
	}
	else if (file_fd < 0) {
		return ERROR;
	}

	// read into buffer
	ssize_t file_bytes = read(file_fd, buffer, buffer_size - 1);
	close(file_fd);
	if (file_bytes < 0) {
		return ERROR;
	}
	// set the null (\0)
	buffer[file_bytes] = '\0';
	return 0;
}
char *buffer_file(const char *path, int32_t *file_size)
{
	// open and check
	int file_fd = open(path, O_RDONLY);
	if (file_fd < 0 && errno == ENOENT) {
		return NULL;
	}
	else if (file_fd < 0) {
		return NULL;
	}

	// check file size
	struct stat file_stat;
	if (fstat(file_fd, &file_stat) < 0 || file_stat.st_size == 0) {
		close(file_fd);
		return NULL;
	}
	// set file_size
	*file_size = file_stat.st_size;
	// create buffer
	char *buffer;
	buffer = malloc(file_stat.st_size);

	// read
	ssize_t read_bytes = read(file_fd, buffer, file_stat.st_size);
	close(file_fd);

	if (read_bytes < 0 || read_bytes != file_stat.st_size) {
		free(buffer);
		return NULL;
	}

	return buffer;
}

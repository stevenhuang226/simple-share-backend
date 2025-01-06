#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>

#include "../config.h"
#include "../include/function.h"
#include "../include/define.h"

void loop_free(char ***array, const int32_t nums)
{
	if (array == NULL) {
		return;
	}
	for (int i = 0; i < nums; i += 1) {
		free((*array)[i]);
		(*array)[i] = NULL;
	}
}
int8_t realloc_array(char ***array, int32_t *size)
{
	*size += ARRAY_ADD_SIZE;
	char **buffer = realloc(*array, *size * sizeof(char *));
	if (buffer == NULL) {
		return MALLOC_ERR;
	}
	*array = buffer;
	return 0;
}

// array >= ARRAY_SIZE or array == NULL requried
int8_t dir_file2arr(char ***array, int32_t *nums_files, const char *path, int8_t clean)
{
	// check directory exists;
	DIR *dir = opendir(path);
	if (dir == NULL) {
		return FILE_NOT_EXISTS;
	}
	// clean the *array and reset *nums_files if needed
	if (clean == 1) {
		loop_free(array, *nums_files);
		free(*array);
		*array = NULL;
		*nums_files = 0;
	}
	// define the array_size (ARRAY_SiZE in include/define.h)
	int32_t array_size = (*nums_files > ARRAY_SIZE) ? *nums_files : ARRAY_SIZE;

	// nums_files > 0 => *array had been alloced , and not need to alloc again,
	// but need to clean the pointer inside
	if (*nums_files > 0 && *array != NULL) {
		loop_free(array, *nums_files);
	}
	else {
		*array = malloc(array_size * sizeof(char *));
	}

	if (*array == NULL) {
		goto clean_ptr_array;
	}

	struct dirent *entry;
	int array_ptr = 0;
	while ((entry = readdir(dir)) != NULL)
	{
		// check type and start with '.' or not
		if (entry->d_type != DT_REG || entry->d_name[0] == '.') {
			continue;
		}
		// if array not big enought, realloc it
		if (array_size <= array_ptr && realloc_array(array, &array_size) < 0) {
			goto clean_array;
		}
		// alloc memory in array[array_ptr] to store file name
		(*array)[array_ptr] = malloc((strlen(entry->d_name) + 1) * sizeof(char));
		if ((*array)[array_ptr] == NULL) {
			goto clean_array;
		}
		// cpy the file name into array[array_ptr]
		strcpy((*array)[array_ptr], entry->d_name);
		array_ptr += 1;
	}
	closedir(dir);

	*nums_files = array_ptr;
	return 0;
clean_array:
	loop_free(array, array_ptr);
	free(*array);
	*array = NULL;
clean_ptr_array:
	closedir(dir);
	*nums_files = ERROR;
	return MALLOC_ERR;
}

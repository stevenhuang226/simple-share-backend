#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "../config.h"
#include "../include/define.h"
#include "../include/function.h"


char **file_array;
int32_t nums_files = 0;

int server_fd;

char *static_html;
char *static_html_header;
char *static_file_header;
char *static_api_header;

char shrd_path[] = SHRD_PATH;

int8_t get_req(const int client_fd, char *req_buffer);
int8_t res_fnajson(const int client_fd);

void cleanup(int lev)
{
	switch(lev) {
		default:
			for (int i = 0; i < nums_files; i += 1) {
				free(file_array[i]);
			}
			free(file_array);
		case STATIC_BUFFER_FAIL:
			free(static_html);
			free(static_html_header);
			free(static_file_header);
			free(static_api_header);
		case BIND_LSN_FAIL:
			close(server_fd);
		case SETSOCK_FAIL:
			if (lev < 0) {
				printf("exit with error...\n");
				exit(EXIT_FAILURE);
			}
			printf("exit...\n");
			exit(EXIT_SUCCESS);
	}
}
void sigclean(int sig)
{
	cleanup(0);
}
int main()
{
	signal(SIGINT, sigclean);
	// set up socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		cleanup(SETSOCK_FAIL);
	}
	// bind and listen
	struct sockaddr_in address = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr.s_addr = INADDR_ANY
	};
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0 ||
		listen(server_fd, 1) < 0) {
		cleanup(BIND_LSN_FAIL);
	}
	// write html, html_header and file_header into buffer
	static_html = malloc(HTML_FILE_SIZE * sizeof(char));
	static_html_header = malloc(HTML_HEADER_FILE_SIZE * sizeof(char));
	static_file_header = malloc(FILE_HEADER_FILE_SIZE * sizeof(char));
	static_api_header = malloc(API_HEADER_FILE_SIZE * sizeof(char));
	if (static_html == NULL || static_html_header == NULL || static_file_header == NULL) {
		cleanup(BIND_LSN_FAIL);
	}
	if (file2buffer(static_html, HTML_FILE_SIZE, HTML_FILE_NAME) < 0 ||
		file2buffer(static_html_header, HTML_HEADER_FILE_SIZE, HTML_HEADER_FILE_NAME) < 0 ||
		file2buffer(static_file_header, FILE_HEADER_FILE_SIZE, FILE_HEADER_FILE_NAME) < 0 ||
		file2buffer(static_api_header, API_HEADER_FILE_SIZE, API_HEADER_FILE_NAME) < 0)
	{
		cleanup(STATIC_BUFFER_FAIL);
	}

	for (;;) {
		// accept to client_fd
		int client_fd = accept(server_fd, NULL, NULL);
		if (client_fd < 0) {
			continue;
		}
		// create buffer for request
		char *req_buffer;
		req_buffer = malloc(REQ_BUFFER_SIZE * sizeof(char));
		if (req_buffer == NULL) {
			close(client_fd);
			continue;
		}
		// receive request and write into buffer
		ssize_t req_bytes = recv(client_fd, req_buffer, REQ_BUFFER_SIZE, 0);
		if (req_bytes < 0) {
			close(client_fd);
			free(req_buffer);
			continue;
		}
		// get the request type (GET || POST || ERROR)
		int8_t type = req_type(req_buffer);
		if (type < 0) {
			close(client_fd);
			free(req_buffer);
			continue;
		}
		else if (type == GET_REQ) {
			get_req(client_fd, req_buffer);
		}
		else if (type == POST_REQ) {
		}

		close(client_fd);
		free(req_buffer);
		// need to free buffer and close client_fd
	}

	return EXIT_SUCCESS;
}
int8_t get_req(const int client_fd, char *req_buffer)
{
	// clean or not generate the file_array
	int8_t clean = (nums_files > ARRAY_SIZE) ? NCLEAN : CLEAN;
	dir_file2arr(&file_array, &nums_files, SHRD_PATH, clean);

	// get file code aka file name in directory
	int32_t file_code = get_handle(req_buffer, &file_array, &nums_files);
	if (file_code == FNA_API_REQ) {
		return res_fnajson(client_fd);
	}
	if (file_code < 0) {
		return ERROR;
	}
	// create header_buffer
	char *header_buffer;
	header_buffer = malloc(FILE_HEADER_FILE_SIZE * sizeof(char));
	if (header_buffer == NULL) {
		return ERROR;
	}
	// cpy the static header data into header_buffer
	strncpy(header_buffer, static_file_header, FILE_HEADER_FILE_SIZE - 1);
	// insert the file name into header
	string_insert(header_buffer,
		FILE_HEADER_FILE_SIZE,
		file_array[file_code],
		FILE_NAME_INSERT);

	// create the full path
	char path[strlen(shrd_path) + strlen(file_array[file_code]) + 1];
	// cpy shared path and  file name into path
	strcpy(path, shrd_path);
	strcpy(&path[strlen(shrd_path)], file_array[file_code]);

	// create file buffer
	char *file_buffer;
	int32_t file_size;
	file_buffer = buffer_file(path, &file_size);
	if (file_buffer == NULL) {
		free(header_buffer);
		return ERROR;
	}
	// insert the file size into header
	content_length_insert(header_buffer,
		FILE_HEADER_FILE_SIZE - strlen(file_array[file_code]),
		file_size,
		strlen(file_array[file_code]) +  FILE_CONTENT_LENGTH_INSERT);
	
	// send to client_fd
	if (send(client_fd, header_buffer, strlen(header_buffer), 0) < 0 ||
		send(client_fd, file_buffer, file_size, 0) < 0)
	{
		free(file_buffer);
		free(header_buffer);
		return ERROR;
	}

	free(file_buffer);
	free(header_buffer);
	return 0;
}
int8_t res_fnajson(const int client_fd)
{
	// create response buffer
	char *res_buffer;
	res_buffer = malloc(API_RES_BUFFER_SIZE * sizeof(char));
	if (res_buffer == NULL) {
		return ERROR;
	}

	// write json into res_buffer
	int32_t content_length;
	if (fna2json(res_buffer, API_RES_BUFFER_SIZE, &content_length, &file_array, &nums_files) < 0) {
		free(res_buffer);
		return ERROR;
	}
	// create header_buffer
	char *header_buffer;
	header_buffer = malloc(API_HEADER_FILE_SIZE * sizeof(char));
	if (header_buffer == NULL) {
		free(res_buffer);
		return ERROR;
	}
	// cpy static api header into header_buffer
	strcpy(header_buffer, static_api_header);
	// insert content length into header_buffer
	content_length_insert(header_buffer,
		API_HEADER_FILE_SIZE - 1,
		content_length,
		API_CONTENT_LENGTH_INSERT);
	// send to cliend
	if (send(client_fd, header_buffer, strlen(header_buffer), 0) < 0 ||
		send(client_fd, res_buffer, content_length, 0) < 0)
	{
		free(header_buffer);
		free(res_buffer);
		return ERROR;
	}
	
	free(header_buffer);
	free(res_buffer);
	return 0;
}

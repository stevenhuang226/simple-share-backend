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
#include "../include/request_types.h"
#include "../include/http_header.h"


char **file_array;
int32_t nums_files = 0;

int server_fd;

char shrd_path[] = SHRD_PATH;

int8_t get_req(const int client_fd, char *req_buffer);
int8_t set_type(const int32_t file_code, struct header *header);
char *set_file(const int32_t file_code, int32_t *file_size);

int8_t put_req(const int client_fd, const char *req_buffer);

void cleanup(int lev)
{
	switch(lev) {
		default:
			for (int i = 0; i < nums_files; i += 1) {
				free(file_array[i]);
			}
			free(file_array);
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
		else if (type == PUT_REQ) {
			put_req(client_fd, req_buffer);
		}

		close(client_fd);
		free(req_buffer);
		// need to free buffer and close client_fd
	}

	return EXIT_SUCCESS;
}
int8_t get_req(const int client_fd, char *req_buffer)
{
	// generate the file_array
	int8_t clean = (nums_files > ARRAY_SIZE) ? NCLEAN : CLEAN;
	dir_file2arr(&file_array, &nums_files, SHRD_PATH, clean);

	// create header struct

	struct header *header;
	header = malloc(sizeof(struct header));
	if (header == NULL) {
		return MALLOC_ERR;
	}
	// set req_stat 200 ok; connection to NULL
	strncpy(header->req_stat, "HTTP/1.1 200 OK", HEADER_req_stat_LEN - 1);
	header->connection[0] = '\0';
	// get file code aka the index for file name in file_array
	int32_t file_code = get_handle(req_buffer, &file_array, &nums_files);
	if (set_type(file_code, header) < 0) {
		free(header);
		return ERROR;
	}
	int32_t data_size;
	char *res_buffer;
	res_buffer = set_file(file_code, &data_size);
	if (res_buffer == NULL) {
		free(header);
		return ERROR;
	}

	header->content_length = data_size;

	char *header_buffer;
	header_buffer = generate_header(header);
	if (header_buffer == NULL) {
		free(header);
		free(res_buffer);
		return ERROR;
	}

	if (send(client_fd, header_buffer, header->size, 0) < 0 ||
		send(client_fd, res_buffer, data_size, 0) < 0)
	{
		free(header);
		free(res_buffer);
		free(header_buffer);
		return ERROR;
	}
	free(header);
	free(res_buffer);
	free(header_buffer);
	return ERROR;
}
int8_t set_type(const int32_t file_code, struct header *header)
{
	if (file_code > 0) {
		strncpy(header->file_name, file_array[file_code], HEADER_file_name_LEN - 1);
		strncpy(header->content_type, "application/octet-stream", HEADER_content_type_LEN - 1);
		return 0;
	}
	header->file_name[0] = '\0';
	if (file_code == FNA_API_REQ) {
		strncpy(header->content_type, "application/json", HEADER_content_type_LEN - 1);
	}
	else if (file_code == HOME_REQ) {
		strncpy(header->content_type, "text/html", HEADER_content_type_LEN - 1);
	}
	else if (file_code == JS_REQ) {
		strncpy(header->content_type, "application/javascript", HEADER_content_type_LEN - 1);
	}
	else {
		return ERROR;
	}
	return 0;
}
char *set_file(const int32_t file_code, int32_t *file_size)
{
	char path[HDLE_PATH_LEN];
	strncpy(path, SHRD_PATH, HDLE_PATH_LEN - 1);

	int shrd_len = strlen(SHRD_PATH);

	if (file_code >= 0) {
		strncpy(&path[shrd_len], file_array[file_code], HDLE_PATH_LEN - shrd_len - 1);
	}
	else if (file_code == HOME_REQ) {
		strncpy(&path[shrd_len], HTML_FILE_NAME, HDLE_PATH_LEN - shrd_len - 1);
	}
	else if (file_code == JS_REQ) {
		strncpy(&path[shrd_len], JS_FILE_NAME, HDLE_PATH_LEN - shrd_len - 1);
	}
	else if (file_code == FNA_API_REQ) {
		char *buffer;
		buffer = malloc(API_RES_BUFFER_SIZE * sizeof(char));
		if (fna2json(buffer, API_RES_BUFFER_SIZE, file_size, &file_array, &nums_files) < 0) {
			free(buffer);
			return NULL;
		}
		return buffer;
	}
	char *file_buffer;
	file_buffer = buffer_file(path, file_size);
	return file_buffer;
}
int8_t put_req(const int client_fd, const char *req_buffer)
{
	struct header *header;
	header = malloc(sizeof(struct header));
	/*
	const char *req_fail = "HTTP/1.1 500 Internal Server Error\r\n"
		"Date: Tue, 08 Jan 2025 12:00:00 GMT\r\n"
		"Server: PotatoServer/1.0\r\n"
		"Content-Length: 0\r\n"
		"Connection: close\r\n"
		"\r\n";
	const char *req_success = "HTTP/1.1 201 Created\r\n"
		"Date: Tue, 08 Jan 2025 12:00:00 GMT\r\n"
		"Server: PotatoServer/1.0\r\n"
		"Content-Length: 0\r\n"
		"Connection: close\r\n"
		"\r\n";
		*/
	header->content_type[0] = '\0';
	header->file_name[0] = '\0';
	header->content_length = 0;
	strncpy(header->connection, "close", HEADER_connection_LEN - 1);

	if (put_handle(req_buffer, SHRD_PATH) < 0) {
		strncpy(header->req_stat, "HTTP/1.1 500 Internal Server Error", HEADER_req_stat_LEN - 1);
	}
	else {
		strncpy(header->req_stat, "HTTP/1.1 201 Created", HEADER_req_stat_LEN - 1);
	}

	char *header_buffer;
	header_buffer = generate_header(header);
	if (header_buffer < 0) {
		free(header);
		return ERROR;
	}
	if (send(client_fd, header_buffer, header->size, 0) < 0) {
		free(header);
		free(header_buffer);
		return ERROR;
	}
	free(header);
	free(header_buffer);
	return 0;
}

#include <sys/types.h>

//fna2json.c
int8_t fna2json(char *buffer, int32_t buffer_size, char ***array, const int32_t *nums_files);
//list_file.c
int8_t dir_file2arr(char ***array, int32_t *nums_files, const char *path, int8_t clean);
// decoder.c
void decode_req(char *buffer);
// request_handle.c
ssize_t content_length(const char *buffer, int32_t max_range, int32_t max_offset);
int8_t req_type(const char *buffer); // return GET_REQ/POST_REQ/-1
int32_t get_handle(const char *req, char ***array, const int32_t *nums_files);
int8_t post_handle(const char *req, const char *path2save);
// file_buff.c
int8_t file2buffer(char *buffer, const int32_t buffer_size, const char *path);
char *buffer_file(const char *path, int32_t *file_size);
// dynamic_insert.c
int8_t content_length_insert(char *buffer, int32_t buffer_size, int32_t content_long, int32_t insert_location);
int8_t string_insert(char *buffer, int32_t buffer_size, const char *file_name, int32_t insert_location);

#define SHRD_PATH "./"
#define PORT 8888

#define REQ_BUFFER_SIZE 0x8000000 // 128*1024*1024 128KB

#define FNA_API_NAME "get_fna"
#define API_HEADER_FILE_NAME "api_header"
#define API_HEADER_FILE_SIZE 1024
#define API_CONTENT_LENGTH_INSERT 0x3f

#define API_RES_BUFFER_SIZE 1024

#define HTML_FILE_NAME "index.html"
#define HTML_FILE_SIZE 4096

#define HTML_HEADER_FILE_NAME "http_headers"
#define HTML_HEADER_FILE_SIZE 4096
#define HEADER_CONTENT_LENGTH_INSERT 0x47

#define FILE_HEADER_FILE_NAME "file_headers"
#define FILE_HEADER_FILE_SIZE 1024
#define FILE_NAME_INSERT 0x62
#define FILE_CONTENT_LENGTH_INSERT 0x74

#define ARRAY_SIZE 64 // default file name array size
#define ARRAY_ADD_SIZE 64 // default add value for file name array

// the maximum set as the path buffer in request handle
#define HDLE_PATH_LEN 8192 // suggest this value bigger then 8192
#define PUT_PATH_LEN 1024 // the maximum path length to save the file(contain SHRD_PATH)

#define CONTENT_FIND_RANGE 8192 // post request handle,
				    // limited when search the content-long
#define MAX_CNT_LEN_OFFSET 128 // the max range to search content-long value
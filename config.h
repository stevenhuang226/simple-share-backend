#define SHRD_PATH // the directory want to share
#define PORT // default port

#define REQ_BUFFER_SIZE 0x8000000 // 128*1024*1024 128KB

#define FNA_API_NAME // the name for the api to get file name array

#define API_RES_BUFFER_SIZE 1024 //default buffer size for api response buffer

#define HTML_FILE_NAME // the default file to return
#define JS_FILE_NAME // the javascript file to return

#define HEADER_BUFFER_SIZE 1024 // default buffer size to create a buffer string

#define ARRAY_SIZE 64 // default file name array size
#define ARRAY_ADD_SIZE 64 // default add value for file name array

// the maximum set as the path buffer in request handle
#define HDLE_PATH_LEN 8192 // suggest this value bigger then 8192
#define PUT_PATH_LEN 1024 // the maximum path length to save the file(contain SHRD_PATH)

#define CONTENT_FIND_RANGE 8192 // post request handle,
				    // limited when search the content-long
#define MAX_CNT_LEN_OFFSET 128 // the max range to search content-long value

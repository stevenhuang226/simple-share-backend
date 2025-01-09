#define SHRD_PATH "./" // the directory want to share
#define PORT 8888 // port

#define REQ_BUFFER_SIZE 0x8000000 // 128*1024*1024 128KB

#define FNA_API_NAME // the path for api to return a json data with directory's file name
#define API_HEADER_FILE_NAME // the header used by api
#define API_HEADER_FILE_SIZE // the api header size ( buffer size )
#define API_CONTENT_LENGTH_INSERT // the location to insert content-length

#define API_RES_BUFFER_SIZE 1024 // the api response buffer, which include json data

#define HTML_FILE_NAME  // the default return file ( path is null )
#define HTML_FILE_SIZE 4096 // default return file buffer size

#define HTML_HEADER_FILE_NAME // the header used by default return file
#define HTML_HEADER_FILE_SIZE 4096 // the header buffer size
#define HEADER_CONTENT_LENGTH_INSERT // where to insert content-length

#define FILE_HEADER_FILE_NAME // header when return regular file
#define FILE_HEADER_FILE_SIZE 1024 // the header size
#define FILE_NAME_INSERT // where to insert returned file's name
#define FILE_CONTENT_LENGTH_INSERT // where to insert content-length

#define ARRAY_SIZE 64 // default file name array size
#define ARRAY_ADD_SIZE 64 // default add value for file name array

// the maximum set as the path buffer in request handle
#define HDLE_PATH_LEN 8192 // suggest this value bigger then 8192
#define PUT_PATH_LEN 1024 // the maximum path length to save the file(contain SHRD_PATH)

#define CONTENT_FIND_RANGE 8192 // post request handle,
				    // limited when search the content-long
#define MAX_CNT_LEN_OFFSET 128 // the max range to search content-long value

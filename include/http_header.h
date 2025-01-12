#define HEADER_content_type_LEN 128
#define HEADER_req_stat_LEN 128
#define HEADER_file_name_LEN 128
#define HEADER_connection_LEN 16

struct header {
	ssize_t size;
	char req_stat[128];
	char content_type[128];
	char file_name[128]; // Content-Disposition: attachment; filename=""
	char connection[32];
	int32_t content_length;
};

// dynamic_header.c
char *generate_header(struct header *header);

#include "HTTPClient.h"
#include "HTTPClientAuth.h"     // Crypto support (Digest, MD5)
#include "HTTPClientString.h"   // String utilities


#define HTTP_MSEC_PER_SEC 1000
#define HTTP_MSEC_TO_SEC(ms) ((((int)(ms)) + HTTP_MSEC_PER_SEC - 1) / HTTP_MSEC_PER_SEC)
#define HTTP_RESULT_SIZE_MAX  4096



int HTTPClientLink(HTTP_SESSION_HANDLE session,
					  char* http_uri,
					  int32_t io_timeout, //in milli-seconds
					  char* post_content_type,
					  char* post_data, int post_len,
					  char* str_head,
					  int32_t retries,
					  int* status_code,
					  char* result_buf, int* buf_size,HTTP_VERB verbtype);


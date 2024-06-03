
#include "HTTPClient.h"

#define HTTP_FILE_NAME_LEN     128

char *HTTPClient_GetFlashDir(void);

int HTTPClient_GetIPFromFile(P_HTTP_SESSION pHTTPSession, char *file_name, struct sockaddr_in  *sockaddr_ipv4, int *ipv4_haved,
                                    char *file_name6, struct sockaddr_in6 *sockaddr_ipv6, int *ipv6_haved);
void HTTPClient_WriteIPToFile(P_HTTP_SESSION pHTTPSession, char *file_name, char *ip_addr);

void HTTPClient_SetFlashDir(const char *flash_dir);


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cutils/sockets.h>

int main() {
    int sock;
    char final_cmd[255] = "0";
    char buf[8] = {0};
    int count;

    strcpy(final_cmd, "cpufreq governor performance");
    
    sock = socket_local_client("optimise", ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);
    if (sock < 0) {
       printf("socket_local_client failed\n");
       return -1;
    }

    count = snprintf(buf, sizeof(buf), "%d", 3);
    int ret = write(sock, buf, count);
    if (ret < 0) {
        perror("write");
	return -1;
    }
	
    ret = write(sock, final_cmd, strlen(final_cmd));
    if (ret < 0) {
        perror("write");
	return -1;
    }
    
    close(sock);
    return 0;
}

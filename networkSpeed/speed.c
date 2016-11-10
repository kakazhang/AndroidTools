/*************************************************************************
	> File Name: speed.c
	> Author: 
	> Mail: 
	> Created Time: 2016年09月14日 星期三 21时53分03秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define RX_PATH "/sys/class/net/wlan0/statistics/rx_bytes"
#define TX_PATH "/sys/class/net/wlan0/statistics/tx_bytes"

#define BUFLEN 32

static void assert(int cond, const char *msg) {
    if (!cond) {
        printf("%s\n", msg);
        exit(-1);
    }
}

int getNetworkBytes(long *rxbytes, long *txbytes) {
    char rxbuf[BUFLEN] = {0};
    char txbuf[BUFLEN] = {0};

    FILE *rxFp = fopen(RX_PATH, "r");
    FILE *txFp = fopen(TX_PATH, "r");
    assert( (rxFp && txFp), "open network speed path failed");

    size_t size = fread(rxbuf, 1, BUFLEN, rxFp);
    if (size > 0)
        *rxbytes = atol(rxbuf) / 1024;

    size = fread(txbuf, 1, BUFLEN, txFp);
    if (size > 0)
        *txbytes = atol(txbuf) / 1024;

    fclose(rxFp);
    fclose(txFp);

    return 0;
}

long compute_speed(long duration) {
    long rxbytes;
    long txbytes;
    getNetworkBytes(&rxbytes, &txbytes);

    sleep(duration);
    
    long new_rxbytes;
    long new_txbytes;

    getNetworkBytes(&new_rxbytes, &new_txbytes);

    long rxspeed = (new_rxbytes - rxbytes) / duration;
    long txspeed = (new_txbytes - txbytes) / duration;

    printf("download speed:%ld kb/s \n", rxspeed);
    printf("upload speed:%ld kb/s \n", txspeed);

    return 0;
}

int main() {
    long duration = 1;
    while(1)
        compute_speed(duration);

    return 0;
}

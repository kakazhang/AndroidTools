#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 256

#define IFACE_LEN 32
#define IFACE_STAT_FMT "/proc/net/xt_qtaguid/iface_stat_fmt"

typedef enum {
    TYPE_WLAN = 0,
	TYPE_RMNET,
	TYPE_LO,
	TYPE_MAX,
} IFACE_TYPE;

typedef struct Stat {
	char iface[IFACE_LEN];

	//received bytes
	uint32_t rx_bytes;
	//received packages
	uint32_t rx_packages;

	//send bytes
	uint32_t tx_bytes;
	//send packages
	uint32_t tx_packages;
} Stat, *pStat;

static void assert(int cond, const char* msg) {
	if (!cond) {
		printf("%s\n", msg);
		exit(-1);
	}
}

static void freeAll(pStat *st) {
	if (st) {
		int i;
		for (i = 0; i < (int)TYPE_MAX; i++) {
			if (st[i]) {
				free(st[i]);
				st[i] = NULL;
			}
		}

		free(st);
	}
}

static void getAllStat(pStat st[]) {
	FILE* fp = NULL;
	char line[LEN] = {0};
	char iface_name[IFACE_LEN] = {0};

	int count = 0;
	
	fp = fopen(IFACE_STAT_FMT, "r");
	if (!fp) {
		perror("fopen");
		return;
	}

	while (fgets(line, LEN, fp)) {	
		uint32_t rx_bytes=0, rx_packages=0, tx_bytes=0, tx_packages=0;
		
		int nmatched = sscanf(line, "%31s %u %u %u %u", iface_name,
			&rx_bytes, &rx_packages, 
			&tx_bytes, &tx_packages);

		if (nmatched == 5) {
			st[count] = (pStat)malloc(sizeof(Stat));
			assert((st[count] != NULL), "no space for pStat");
			
			strcpy(st[count]->iface, iface_name);
			st[count]->rx_bytes = rx_bytes;
			st[count]->rx_packages = rx_packages;
			st[count]->tx_bytes = tx_bytes;
			st[count]->tx_packages = tx_packages;

			memset(line, 0, LEN);
			memset(iface_name, 0, IFACE_LEN);
			count++;
			if (count == TYPE_MAX)
				break;
		}
	}

	fclose(fp);
}

void list_ifaces() {
	pStat *st = (pStat *)malloc(sizeof(pStat) * (int)TYPE_MAX);
	assert((st != NULL), "no space for pStat array");

	int i;
	int num = (int)TYPE_MAX;
	for (i = 0; i < num; i++)
		st[i] = NULL;

	getAllStat(st);

	for (i = 0; i < num; i++) {
		if (st[i] != NULL) {
			printf("%s\n", st[i]->iface);
		}
	}

	freeAll(st);
}

uint32_t get_rx_bytes(const char* iface) {
	
	uint32_t rx_bytes = 0;
	
	pStat *st = (pStat *)malloc(sizeof(pStat) * (int)TYPE_MAX);
	assert((st != NULL), "no space for pStat array");

	int i;
	int num = (int)TYPE_MAX;
	for (i = 0; i < num; i++)
		st[i] = NULL;

	getAllStat(st);
	
	for (i = 0; i < num; i++) {
		if (st[i] != NULL && !strncmp(iface, st[i]->iface, strlen(iface))) {
			rx_bytes = st[i]->rx_bytes;
			break;
		}
	}

	freeAll(st);
	
	return rx_bytes;
}

uint32_t get_tx_bytes(const char* iface) {
	uint32_t tx_bytes = 0;
	
	pStat *st = (pStat *)malloc(sizeof(pStat) * (int)TYPE_MAX);
	assert((st != NULL), "no space for pStat array");

	int i;
	int num = (int)TYPE_MAX;
	for (i = 0; i < num; i++)
		st[i] = NULL;

	getAllStat(st);
	
	for (i = 0; i < num; i++) {
		if (st[i] != NULL && !strncmp(iface, st[i]->iface, strlen(iface))) {
			tx_bytes = st[i]->tx_bytes;
			break;
		}
	}

	freeAll(st);
	
	return tx_bytes;
}


int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Too few arugments\n");
		return -1;
	}

	if (!strcmp(argv[1], "listall")) {
		list_ifaces();
	} else if (!strcmp(argv[1], "all_rx_bytes")) {
		uint32_t rx_bytes = get_rx_bytes(argv[2]);
		printf("all rx_bytes:%u bytes\n",rx_bytes);
	} else if (!strcmp(argv[1], "all_tx_bytes")) {
		uint32_t tx_bytes = get_tx_bytes(argv[2]);
		printf("all tx_bytes:%u bytes\n", tx_bytes);
	}

	return 0;
}

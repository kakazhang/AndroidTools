#include <stdio.h>
#include "../inc/common.h"

void showTargets(int targets[], int len) {
    for (int i = 0; i < len; i++)
        printf("targets[%d]=%d ", i,targets[i]);
    printf("\n");
}

//using namespace android;

int main() {
    int targets1[3] = {HOST_ID(PRO_DEVICE_TYPE_PC,0),HOST_ID(PRO_DEVICE_TYPE_PC,1),HOST_ID(PRO_DEVICE_TYPE_APP,1)};
    int targets2[1] = {HOST_ID(PRO_DEVICE_TYPE_REMOTE_CONTROL,0)};
    int targets3[4] = {HOST_ID(PRO_DEVICE_TYPE_GS_DM368,1),HOST_ID(PRO_DEVICE_TYPE_GS_OFDM,7),
                        HOST_ID(PRO_DEVICE_TYPE_GS_OFDM,0),HOST_ID(PRO_DEVICE_TYPE_APP,0)};
    int targets4[1] = {HOST_ID(PRO_DEVICE_TYPE_APP,1)};

    showTargets(targets1, 3);
    showTargets(targets2, 1);
    showTargets(targets3, 4);
    showTargets(targets4, 1);

    return 0;
}

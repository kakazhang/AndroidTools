#include <hardware/hardware.h>
#include <hardware/power.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_powersaver(int on) {
    power_module_t *power = NULL;
    int success = hw_get_module("powersaver", &power);
    if (success < 0) {
        printf("open module failed\n");
        return;
    }

    if (power->init) {
         power->init(power);
    }

    if (power->setInteractive) {
         power->setInteractive(power, on);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("At least 2 arguments\n");
        return -1;
    }

    int on = atoi(argv[1]);
    test_powersaver(on);

    return 0;
}

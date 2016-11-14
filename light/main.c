#include <hardware/hardware.h>
#include <hardware/lights.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_light(int color) {
    hw_module_t *backlight = NULL;

    int success = hw_get_module("backlight", &backlight);
    if (success < 0) {
        printf("open module failed\n");
        return;
    }

    struct light_device_t *light_dev = NULL;
    int ret = backlight->methods->open(backlight, "backlight", (hw_device_t *)&light_dev);
    if (ret < 0) {
        printf("open device failed\n");
        return;
    }

    struct light_state_t state = {
        .color = color,
    };

    light_dev->set_light(light_dev, &state);

    light_dev->common.close(light_dev);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("At least 2 arguments\n");
        return -1;
    }

    int color = atoi(argv[1]);
    test_light(color);

    return 0;
}

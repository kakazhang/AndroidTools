
#define LOG_TAG "Fake_Vold"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <cutils/log.h>
#include <fs_mgr.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "NetlinkManager.h"
#include "VolumeManager.h"
#include "DirectVolume.h"
#include "Loop.h"

#define FSTAB "/fstab."
#define PATH_LEN 256

#define VOL_NONREMOVABLE   0x1
#define VOL_ENCRYPTABLE    0x2
#define VOL_PRIMARY        0x4
#define VOL_PROVIDES_ASEC  0x8

struct fstab* fstab;


static int process_config(VolumeManager* vm);

//atic void create_partition();

int main(int argc, char** argv) {
#if 0

	NetlinkManager* nm = NetlinkManager::Instance();
	if (nm != NULL)
		nm->start();
#else
	VolumeManager *vm;
	vm = VolumeManager::Instance();
	process_config(vm);
	vm->listVolume();
	//process_config(vm);
	//char fsPath[] = {"/data/local/tmp/src"};
	//char imageName[] = {"/data/local/tmp/kaka.img"};
	//vm->create_partition(fsPath, imageName);
	delete vm;
#endif 

	return 0;
}

static int process_config(VolumeManager* vm)
{
    char fstab_filename[PROPERTY_VALUE_MAX + sizeof(FSTAB)];
    char propbuf[PROPERTY_VALUE_MAX];
    int i;
    int ret = -1;
    int flags;

    property_get("ro.hardware", propbuf, "");
    snprintf(fstab_filename, sizeof(fstab_filename), FSTAB"%s", propbuf);

    fstab = fs_mgr_read_fstab(fstab_filename);
    if (!fstab) {
        SLOGE("failed to open %s\n", fstab_filename);
        return -1;
    }

    /* Loop through entries looking for ones that vold manages */
    for (i = 0; i < fstab->num_entries; i++) {
        if (fs_mgr_is_voldmanaged(&fstab->recs[i])) {
            DirectVolume *dv = NULL;
            flags = 0;

            /* Set any flags that might be set for this volume */
            if (fs_mgr_is_nonremovable(&fstab->recs[i])) {
                flags |= VOL_NONREMOVABLE;
            }
            if (fs_mgr_is_encryptable(&fstab->recs[i])) {
                flags |= VOL_ENCRYPTABLE;
            }
            /* Only set this flag if there is not an emulated sd card */
            if (fs_mgr_is_noemulatedsd(&fstab->recs[i]) &&
                !strcmp(fstab->recs[i].fs_type, "vfat")) {
                flags |= VOL_PROVIDES_ASEC;
            }
            dv = new DirectVolume(vm, &(fstab->recs[i]), flags);

            /*if (dv->addPath(fstab->recs[i].blk_device)) {
                printf("Failed to add devpath %s to volume %s",
                      fstab->recs[i].blk_device, fstab->recs[i].label);
                goto out_fail;
            }*/

            vm->addVolume(dv);
        }
    }

    ret = 0;

out_fail:
    return ret;
}


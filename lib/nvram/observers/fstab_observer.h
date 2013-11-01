/*
 * fstab_observer.h
 *
 *  Created on: 03.12.2012
 *      Author: yaroslav
 */

#ifndef FSTAB_OBSERVER_H_
#define FSTAB_OBSERVER_H_

#include "nvram_observer.h"
#include "conf_parser.h" //struct ParsedRecord

#define HANDLE_ONLY_FSTAB_SECTION get_fstab_observer()

#define FSTAB_SECTION_NAME         "fstab"
#define FSTAB_PARAM_CHANNEL_KEY    "channel"
#define FSTAB_PARAM_MOUNTPOINT_KEY "mountpoint"
#define FSTAB_PARAM_ACCESS_KEY     "access"
#define FSTAB_PARAM_REMOVABLE         "removable"

#define FSTAB_VAL_ACCESS_READ      "ro"  /*for injecting files into FS*/
#define FSTAB_VAL_ACCESS_WRITE     "wo"  /*for copying files into image*/

#define FSTAB_VAL_REMOVABLE_YES       "yes"
#define FSTAB_VAL_REMOVABLE_NO        "no"

/* If mount_stage is equal to FSTAB_MOUNT_FIRST_STAGE then always return 1,
 * if fstab_stage value is equal to FSTAB_REMOUNT_STAGE then return 1 only 
 * in case if removable_record is 1 */
#define IS_NEED_TO_HANDLE_FSTAB_RECORD(mount_stage, removable_record)	\
    ((EFstabMountWaiting)==(mount_stage))? 1:				\
    ((EFstabMountComplete)==(mount_stage)) && 0!=(removable_record)? 1 : 0

enum {EFstabMountWaiting, EFstabMountProcessing, EFstabMountComplete};
struct FstabRecordContainer{
    struct ParsedRecord mount;
    int mount_status; /* EFstabMountWaiting, EFstabMountProcessing, EFstabMountComplete */
};

/*new fstab observer is derived from nvram observer*/
struct FstabObserver {
    struct MNvramObserver base;
    /*derived function "handle_nvram_record(...)"*/
    /*export fs contents into tar archive, in according to fstab record with access=rw*/
    void (*mount_export)(struct FstabObserver* observer);
    /*import tar archive  into maountpoint path, related to fstab record with access=ro*/
    void (*mount_import)(struct FstabObserver* observer, 
			 struct FstabRecordContainer* record);
    /*Say to removable mounts that they need to be remounted*/
    void (*reset_removable)(struct FstabObserver* observer);
    /* Locate ParsedRecord with mountpoint and mount status matched
     * @param alias 
     * @param mount_status 
     * @return index of matched record, -1 if not located*/
    struct FstabRecordContainer* (*locate_postpone_mount)(struct FstabObserver* observer, 
						  const char* alias, int mount_status);
    /*new fields for postponed lazy mount, tar export at exit;
     *expected that array will get new items during fstab handling*/
    struct FstabRecordContainer* postpone_mounts_array;
    int postpone_mounts_count;
};

/*get static interface object not intended to destroy after using*/
struct FstabObserver* get_fstab_observer();

#endif /* FSTAB_OBSERVER_H_ */

/*
 * Copyright (c) 2013, 2019, 2022-2023, BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */


#ifndef _FAT_FS_H_
#define _FAT_FS_H_

#define SECTOR_SIZE     512             // standard block size

#define ENT_UNUSED      0xE5
#define ENT_END         0x00

#define TYPE_FAT12      12
#define TYPE_FAT16      16
#define TYPE_FAT32      32

#define MIN(a,b)        (((a) < (b)) ? (a) : (b))

#define GET_CLUSTER(x)  ((x)->clust_lo | ((x)->clust_hi << 16))

/* partition structure */
typedef struct partition
{
    uint8_t boot_ind;           // boot indicator
    uint8_t beg_head;           // begin head
    uint8_t begin_sect;         // begin sector
    uint8_t beg_cylinder;       // begin cylinder
    uint8_t os_type;            // partition type
    uint8_t end_head;           // end head
    uint8_t end_sect;           // end sector
    uint8_t end_cylinder;       // end cylinder
    uint8_t part_offset[4];     // startsector number
    uint8_t part_size[4];       // partition size in sectors
} partition_t;

/* Master Boot Record */
typedef struct fat_mbr
{
    uint8_t         pad[446];           // fill bytes
    partition_t     part_entry[4];      // partition entries
    uint16_t        sign;               // signature (0xAA55)
} fat_mbr_t;

/* Boot Sector (valid only for FAT12 and FAT16) */
typedef struct fat_bpb
{
    uint8_t     jump[3];
    uint8_t     oem_name[8];
    uint8_t     sec_size[2];
    uint8_t     sec_per_clus;
    uint8_t     num_rsvd_secs[2];
    uint8_t     num_fats;
    uint8_t     num_root_ents[2];
    uint8_t     num16_secs[2];
    uint8_t     media;
    uint8_t     num16_fat_secs[2];
    uint8_t     sec_per_trk[2];
    uint8_t     num_heads[2];
    uint8_t     num_hidden_sec[4];
    uint8_t     num32_secs[4];
    uint8_t     drv_num;
    uint8_t     cur_head;
    uint8_t     boot_sig;
    uint8_t     vol_id[4];
    uint8_t     vol_label[11];
    uint8_t     sys_type[8];
    uint8_t     pad[448];
    uint8_t     sig[2];
} fat_bpb_t;

/* Boot Sector (valid only for FAT32) */
typedef struct fat_bpb32
{
    uint8_t     jump[3];
    uint8_t     oem_name[8];
    uint8_t     sec_size[2];
    uint8_t     sec_per_clus;
    uint8_t     num_rsvd_secs[2];
    uint8_t     num_fats;
    uint8_t     num_root_ents[2];
    uint8_t     num16_secs[2];
    uint8_t     media;
    uint8_t     num16_fat_secs[2];
    uint8_t     sec_per_trk[2];
    uint8_t     num_heads[2];
    uint8_t     num_hidden_sec[4];
    uint8_t     num32_secs[4];
    uint8_t     num32_fat_secs[4];
    uint8_t     ext_flags[2];
    uint8_t     version[2];
    uint8_t     root_clus[4];
    uint8_t     fsinfo_sec[2];
    uint8_t     backup_boot_sec[2];
    uint8_t     reserved[12];
    uint8_t     drv_num;
    uint8_t     cur_head;
    uint8_t     boot_sig;
    uint8_t     vol_id[4];
    uint8_t     vol_label[11];
    uint8_t     sys_type[8];
    uint8_t     code[10];
    uint8_t     pad[410];
    uint8_t     sig[2];
} fat_bpb32_t;

/* file system information */
typedef struct fat_fs_info
{
    uint32_t    fs_offset;          // boot sector offset
    uint32_t    fat_type;           // type of FAT (12, 16)
    uint32_t    total_sectors;      // total number of sectors
    uint32_t    root_dir_sectors;   // number of sectors for root irectory
    uint32_t    reserved_sectors;   // reserved sectors
    uint32_t    data_sectors;       // number of sectors to store data to
    uint32_t    fat_size;           // fat size;
    uint32_t    number_of_fats;     // number of fats
    uint32_t    cluster_size;       // sectors per cluster
    uint32_t    count_of_clusters;  // number of clusters

    uint32_t    fat1_start;         // start of first FAT
    uint32_t    fat2_start;         // start of seconf FAT
    uint32_t    root_dir_start;     // start of root directory
    uint32_t    root_entry_count;   // number of root directory entries
    uint32_t    cluster2_start;     // start of first data cluster

    void        *device;            // pointer to device structure
} fat_fs_info_t;

#define FAT_COMMON_BUF_SIZE     (2 * SECTOR_SIZE)
#define FAT_FS_INFO_BUF_SIZE    (sizeof(fat_fs_info_t))

/* directory entry */
typedef struct fat_dir_entry
{
    uint8_t     short_name[11];
    uint8_t     attrib;
    uint8_t     ntres;
    uint8_t     crt_time_tenth;
    uint16_t    crt_time;
    uint16_t    crt_date;
    uint16_t    lst_acc_date;
    uint16_t    clust_hi;
    uint16_t    wrt_time;
    uint16_t    wrt_date;
    uint16_t    clust_lo;
    uint32_t    size;
} fat_dir_entry_t;

#define LAST_LONG_ENTRY 0x40

/* file system function */
extern int fat_copy_named_file(blkdev_t *blkdev, uint8_t *buf, char *name);

#endif /* #ifndef _FAT_FS_H_ */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/fat-fs.h $ $Rev: 975396 $")
#endif

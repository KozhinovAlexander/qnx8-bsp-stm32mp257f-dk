/*
 * Copyright (c) 20013, 2015, 2019, 2022-2023, BlackBerry Limited.
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

#include "ipl.h"
#include "fat-fs.h"

/* file system information structure */
static fat_fs_info_t   *fs_info;

/* common block buffer */
static uint8_t  *blk_buf;
static uint8_t  *blk;
static uint32_t g_fat_sector;

static inline uint32_t get_word(const uint8_t *const dat)
{
    return (dat[0] + (uint32_t)(dat[1] << 8));
}

static inline uint32_t get_long(const uint8_t *const dat)
{
    return (dat[0] + (uint32_t)(dat[1] << 8) + (uint32_t)(dat[2] << 16) + (uint32_t)(dat[3] << 24));
}

/* memory length */
static int
_strlen(const char *const str)
{
    const char *s = (char *)str;
    int len = 0;

    if (0 == str) return 0;

    while ('\0' != *s++) {
        ++len;
    }

    return len;
}

/* reads a sector relative to the start of the block device */
static int
read_sector(blkdev_t *const blkdev, const unsigned blkno, void *const buf, const unsigned blkcnt)
{
    return blkdev->blkrw((void *)blkdev, buf, blkno, blkcnt, 1);
}

/* reads a sector relative to the start of the partition 0 */
static int
read_fsector(blkdev_t *const blkdev, const unsigned sector, void *const buf, const unsigned sect_cnt)
{
    return read_sector(blkdev, sector + fs_info->fs_offset, buf, sect_cnt);
}

/* detects the type of FAT */
static int
fat_detect_type(const fat_bpb_t *const bpb)
{
    const fat_bpb32_t *const bpb32 = (fat_bpb32_t *)bpb;

    if (get_word(bpb->sig) != 0xaa55) {
        kprintf("BPB signature is wrong\n");
        return -1;
    }

    {
        int rc, bs, ns = 0;

        bs = (int)get_word(bpb->sec_size);
        rc = (int)get_word(bpb->num_root_ents) * 32 + bs - 1;
        for (; rc >= bs; rc -= bs) {
            ns++;
        }
        fs_info->root_dir_sectors = (uint32_t)ns;
    }

    fs_info->fat_size = get_word(bpb->num16_fat_secs);
    if (fs_info->fat_size == 0) {
        fs_info->fat_size = get_long(bpb32->num32_fat_secs);
    }

    fs_info->total_sectors = get_word(bpb->num16_secs);
    if (fs_info->total_sectors == 0) {
        fs_info->total_sectors =get_long(bpb->num32_secs);
    }

    fs_info->number_of_fats = bpb->num_fats;
    fs_info->reserved_sectors = get_word(bpb->num_rsvd_secs);

    fs_info->data_sectors = fs_info->total_sectors -
        (fs_info->reserved_sectors + fs_info->number_of_fats * fs_info->fat_size + fs_info->root_dir_sectors);

    fs_info->cluster_size = bpb->sec_per_clus;

    {
        int ds, cs, nc = 0;

        ds = (int)fs_info->data_sectors;
        cs = (int)fs_info->cluster_size;
        for (; ds >= cs; ds -= cs) {
            nc++;
        }
        fs_info->count_of_clusters = (uint32_t)nc;
    }

    fs_info->root_entry_count = get_word(bpb->num_root_ents);
    fs_info->fat1_start = fs_info->reserved_sectors;
    fs_info->fat2_start = fs_info->fat1_start + fs_info->fat_size;
    fs_info->root_dir_start = fs_info->fat2_start + fs_info->fat_size;
    fs_info->cluster2_start = fs_info->root_dir_start + ((fs_info->root_entry_count * 32) + (512 - 1)) / 512;

    if (fs_info->count_of_clusters < 4085) {
        return 12;
    } else if (fs_info->count_of_clusters < 65525) {
        return 16;
    } else {
        fs_info->root_dir_start = get_long(bpb32->root_clus);
        return 32;
    }

    return -1;
}

/* reads the Master Boot Record to get partition information */
static int
fat_parse_mbr(blkdev_t *const blkdev, const fat_mbr_t *const mbr, fat_bpb_t *const bpb)
{
    const partition_t *const pe = &(mbr->part_entry[0]);
    uint16_t    sign;

    sign = mbr->sign;
    if ( sign != 0xaa55) {
        kprintf("   Error: MBR signature (%x) is wrong\n", sign);
        return -1;
    }

    /*
     * We don't want to include fancy code in IPL to elaborate all the
     * complexity of CHS/LBA conversion to validate MBR
     * Instead, we go ahead to assume it's MBR sector if it doesn't have
     * obvious problems.
     */
    if (get_long(pe->part_size) == 0) {
        kprintf("   Error: partition 0 in MBR is invalid\n");
        return -1;
    }

    /* Assume valid MBR and valid partition 0 */
    fs_info->fs_offset = get_long(pe->part_offset);

    if (blkdev->verbose) {
        kprintf("Partition entry 0:\n");
        kprintf("        Boot Indicator: 0x%x\n", (uint32_t)mbr->part_entry[0].boot_ind);
        kprintf("        Partition type: 0x%x\n", (uint32_t)pe->os_type);
        kprintf("        Begin C_H_S:    %d %d %d\n", (uint32_t)pe->beg_head, (uint32_t)pe->begin_sect, (uint32_t)pe->beg_cylinder);
        kprintf("        END C_H_S:      %d %d %d\n", (uint32_t)pe->end_head, (uint32_t)pe->end_sect, (uint32_t)pe->end_cylinder);
        kprintf("        Start sector:   %d\n", get_long(pe->part_offset));
        kprintf("        Partition size: %d\n", get_long(pe->part_size));
    }

    /* read BPB structure */
    if (read_fsector(blkdev, 0, (unsigned char *)bpb, 1)) {
        kprintf("   Error: cannot read BPB\n");
        return -1;
    }

    /* detect the FAT type of partition 0 */
    fs_info->fat_type = (uint32_t)fat_detect_type(bpb);
    if (fs_info->fat_type == (uint32_t)-1) {
        kprintf("   error detecting BPB type\n");
        return -1;
    }

    return 0;
}

/* converts a cluster number into sector numbers to the partition 0 */
static uint32_t
cluster2fsector(const uint32_t cluster)
{
    return fs_info->cluster2_start + (cluster - 2) * fs_info->cluster_size;
}

/* gets the entry of the FAT for FAT12 */
static uint32_t
get_fat_entry12(blkdev_t *const blkdev, const unsigned cluster)
{
    const uint32_t fat_sector = fs_info->fat1_start + ((cluster + (cluster / 2)) / SECTOR_SIZE);
    uint32_t fat_offs = (cluster + (cluster / 2)) % SECTOR_SIZE;

    if (0 != read_fsector(blkdev, fat_sector, blk, 2)) return 0;

    if (cluster & 0x1) {
        return (get_word(&blk[fat_offs]) >> 4);
    } else {
        return (get_word(&blk[fat_offs]) & 0xfff);
    }
}

/* gets the entry of the FAT for FAT16 */
static uint32_t
get_fat_entry16(blkdev_t *const blkdev, const uint32_t cluster)
{
    const uint32_t fat_sector = fs_info->fat1_start + ((cluster * 2) / SECTOR_SIZE);
    const uint32_t fat_offs = (cluster * 2) % SECTOR_SIZE;
    uint8_t *const data_buf = blk_buf;

    if (g_fat_sector == fat_sector) {
        return *(uint16_t *)(data_buf + fat_offs);
    }
    if (0 != read_fsector(blkdev, fat_sector, data_buf, 1)) {
        return 0;
    }

    g_fat_sector = fat_sector;

    return *(uint16_t *)(data_buf + fat_offs);
}

/* gets the entry of the FAT for FAT32 */
static uint32_t
get_fat_entry32(blkdev_t *const blkdev, const unsigned cluster)
{
    const uint32_t fat_sector = fs_info->fat1_start + ((cluster * 4) / SECTOR_SIZE);
    const uint32_t fat_offs = (cluster * 4) % SECTOR_SIZE;
    uint8_t *const data_buf = blk_buf;

    if (g_fat_sector == fat_sector) {
        return (*(uint32_t *)(data_buf + fat_offs)) & 0x0fffffff;
    }
    if (0 != read_fsector(blkdev, fat_sector, data_buf, 1)) {
        return 0;
    }

    g_fat_sector = fat_sector;

    return (*(uint32_t *)(data_buf + fat_offs)) & 0x0fffffff;
}

/* gets the entry of the FAT for the given cluster number */
static uint32_t
fat_get_fat_entry(blkdev_t *const blkdev, const uint32_t cluster)
{
    switch (fs_info->fat_type) {
    case TYPE_FAT12:
        return get_fat_entry12(blkdev, cluster);
    case TYPE_FAT16:
        return get_fat_entry16(blkdev, cluster);
    case TYPE_FAT32:
        return get_fat_entry32(blkdev, cluster);
        default:
            return 0;
    }
}

/* checks for end of file condition */
static int
end_of_file(const uint32_t cluster)
{
    switch (fs_info->fat_type) {
    case TYPE_FAT12:
        return ((cluster == 0x0ff8) || (cluster == 0x0fff));
    case TYPE_FAT16:
        return ((cluster == 0xfff8) || (cluster == 0xffff));
    case TYPE_FAT32:
        return ((cluster == 0x0ffffff8) || (cluster == 0x0fffffff));
        default:
            return 1;
    }
}

/* copy a file from to a memory location */
static int
fat_copy_file(blkdev_t *const blkdev, const uint32_t cluster, const uint32_t size, uint8_t *buf)
{
    int         result, txf;
    uint32_t    prev_c, next_c, curr_c;
    int         sz = (int)size;
    const int   cbytes = (int)fs_info->cluster_size * SECTOR_SIZE;

    g_fat_sector = (uint32_t)-1;

    /*
    *Note that this impl assume the following:
    * 1) The max DMA transfer size is bigger than the max consolidate transfer size
    * Otherwise, we need to break down into smaller transfer.
    * 2) we always do at least one whole cluster transfer. This might overwrite the client buffer, but
    * since this is purely used for IPL, we don't care about that now.
    */
    curr_c = cluster;
    while (sz > 0) {
        txf = cbytes;
        prev_c = curr_c;
        while (sz > txf) {
            //try consolidate contigus entry;
            next_c = fat_get_fat_entry(blkdev, curr_c);
            if (next_c == (curr_c+1)) {
                txf +=cbytes;
                curr_c = next_c;
            } else {
                curr_c = next_c;
                break;
            }
        }

        //read the contig cluster out
        result = read_fsector(blkdev, cluster2fsector(prev_c), buf, (unsigned int)txf / SECTOR_SIZE);
        if (result != 0) return result;
        sz  -= txf;
        buf += txf;
    }

    return 0;
}

/* copy file by name (FAT12/16) */
static int
copy_named_file_fat16(blkdev_t *const blkdev, uint8_t *const buf, const char *const name)
{
    uint32_t    dir_sector = fs_info->root_dir_start;
    int         i, len;
    int         ent = 0;

    while (dir_sector < fs_info->root_dir_start + fs_info->root_dir_sectors) {
        if (0 != read_fsector(blkdev, dir_sector, blk, 1)) {
            kprintf("read_fsector failed!\n");
            return -1;
        }

        ent = 0;
        while (ent < SECTOR_SIZE / sizeof(fat_dir_entry_t)) {
            fat_dir_entry_t *const dir_entry = (fat_dir_entry_t *)&(blk[ent * sizeof(fat_dir_entry_t)]);

            if (dir_entry->short_name[0] == ENT_END) break;

            if (dir_entry->short_name[0] == ENT_UNUSED) {
                ent++;
                continue;
            }

            len = (_strlen (name) < 11) ? _strlen (name) : 11;
            for (i = 0; i < len; i++) {
                if (name[i] != (char)dir_entry->short_name[i]) break;
            }

            if ((i < len) || (dir_entry->clust_lo == 0)) {
                ent++;
                continue;
            }

            return fat_copy_file(blkdev, dir_entry->clust_lo, dir_entry->size, buf);
        }

        if (ent < SECTOR_SIZE / sizeof(fat_dir_entry_t)) break;

        dir_sector++;
    }

    return -1;
}

/* copy file by name (FAT32) */
static int
copy_named_file_fat32(blkdev_t *const blkdev, uint8_t *const buf, const char *const name)
{
    uint32_t       dir_clust = fs_info->root_dir_start;
    const uint32_t clust_sz = fs_info->cluster_size;
    uint32_t       dir_sector;
    int            i, len;
    int            ent = 0;

    while (!end_of_file(dir_clust)) {
        uint32_t sub_sect = 0;
        dir_sector = cluster2fsector(dir_clust);

        while (sub_sect < clust_sz) {
            if (0 != read_fsector(blkdev, dir_sector + sub_sect, blk, 1)) {
                return -1;
            }

            ent = 0;
            while (ent < SECTOR_SIZE / sizeof(fat_dir_entry_t)) {
                fat_dir_entry_t *const dir_entry = (fat_dir_entry_t *)&(blk[ent * sizeof(fat_dir_entry_t)]);

                if (dir_entry->short_name[0] == ENT_END) break;

                if (dir_entry->short_name[0] == ENT_UNUSED) {
                    ent++;
                    continue;
                }

                len = (_strlen (name) < 11) ? _strlen (name) : 11;
                for (i = 0; i < len; i++) {
                    if (name[i] != (char)dir_entry->short_name[i]) break;
                }

                if ((i < len) || (GET_CLUSTER(dir_entry) == 0)) {
                    ent++;
                    continue;
                }
                return fat_copy_file(blkdev, (uint32_t)GET_CLUSTER(dir_entry), dir_entry->size, buf);
            }

            if (ent < SECTOR_SIZE / sizeof(fat_dir_entry_t)) break;

            sub_sect++;
        }

        dir_clust = fat_get_fat_entry(blkdev, dir_clust);
    }

    return -1;
}

/* copy file by name */
static int
_fat_copy_named_file(blkdev_t *const blkdev, uint8_t *const buf, const char *const name)
{
    switch (fs_info->fat_type) {
    case TYPE_FAT12:
    case TYPE_FAT16:
        if (0 != copy_named_file_fat16(blkdev, buf, name)) {
            kprintf("copy_named_file_fat16 failed!\n");
            return -1;
        }
        return 0;
    case TYPE_FAT32:
        if (0 != copy_named_file_fat32(blkdev, buf, name)) {
            kprintf("copy_named_file_fat32 failed!\n");
            return -1;
        }
        return 0;
        default:
    kprintf("Unsupport file system: %x\n", fs_info->fat_type);
    return -1;
    }
}

int
fat_copy_named_file(blkdev_t *const blkdev, uint8_t *const buf, char *const name)
{
    fat_mbr_t *mbr;
    fat_bpb_t *bpb;

    if (!blkdev || (blkdev->blkrw == (void *)0)) {
        return -1;
    }

    if (blkdev->alloc_dmabuf != (void *)0) {
        blk_buf = blkdev->alloc_dmabuf(FAT_FS_INFO_BUF_SIZE + FAT_COMMON_BUF_SIZE);
        if (blk_buf == (void *)0) {
            kprintf("%s: failed to allocate DMA buffer\n");
            return -1;
        }
    } else {
        /* buffers */
        static uint32_t fatbuf[(FAT_FS_INFO_BUF_SIZE + FAT_COMMON_BUF_SIZE) / sizeof(uint32_t)] = {0};
        blk_buf = (uint8_t *)&fatbuf[0];
    }
    fs_info = (fat_fs_info_t *)&blk_buf[FAT_COMMON_BUF_SIZE];

    blk = blk_buf;
    mbr = (fat_mbr_t *)&blk[0];
    bpb = (fat_bpb_t *)&blk[SECTOR_SIZE];

    /* read MBR from sector 0 */
    if (0 != read_sector(blkdev, 0, mbr, 1)) {
        kprintf("   Error: cannot read MBR\n");
        return -1;
    }

    /*
     * If we fail to read the file based on the assumption that
     * sector 0 is MBR, we will try BPB
     */
    if ((fat_parse_mbr(blkdev, mbr, bpb) == 0) &&
        (_fat_copy_named_file(blkdev, buf, name) == 0)) {
        return 0;
    }

    /* Invalid MBR, trying the BPB at the first sector */
    fs_info->fs_offset = 0;

    /* detect the FAT type of partition 0 */
    fs_info->fat_type = (uint32_t)fat_detect_type((fat_bpb_t *)mbr);
    if (fs_info->fat_type== (uint32_t)-1) {
        kprintf("   error detecting BPB type\n");
        return -1;
    }

    return (_fat_copy_named_file(blkdev, buf, name));
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/fat-fs.c $ $Rev: 975396 $")
#endif

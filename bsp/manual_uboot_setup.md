1. Format partition 8 as FAT32 (if needed)

U-Boot shall show following partitons after `make sdcard_provision` command:

```sh
STM32MP> part list mmc 0

Part    Start LBA       End LBA         Name
        Attributes
        Type GUID
        Partition GUID
  1     0x00000022      0x00000221      "fsbla1"
        attrs:  0x0000000000000000
        type:   8da63339-0007-60c0-c436-083ac8230908
                (8da63339-0007-60c0-c436-083ac8230908)
        guid:   e091b5dd-3628-4957-91ae-e767d6b14368
  2     0x00000222      0x00000421      "fsbla2"
        attrs:  0x0000000000000000
        type:   8da63339-0007-60c0-c436-083ac8230908
                (8da63339-0007-60c0-c436-083ac8230908)
        guid:   e28ea0d4-f635-48f2-856d-0ca58ba551a3
  3     0x00000422      0x00000621      "metadata1"
        attrs:  0x0000000000000000
        type:   8a7a84a0-8387-40f6-ab41-a8b9a5a60d23
                (8a7a84a0-8387-40f6-ab41-a8b9a5a60d23)
        guid:   34f0e87c-d4a8-410c-a3e7-930c49712a18
  4     0x00000622      0x00000821      "metadata2"
        attrs:  0x0000000000000000
        type:   8a7a84a0-8387-40f6-ab41-a8b9a5a60d23
                (8a7a84a0-8387-40f6-ab41-a8b9a5a60d23)
        guid:   342c87fb-2c3e-4c52-863b-1bdcbc988df1
  5     0x00000822      0x00002821      "fip-a"
        attrs:  0x0000000000000000
        type:   19d5df83-11b0-457b-be2c-7559c13142a5
                (19d5df83-11b0-457b-be2c-7559c13142a5)
        guid:   4fd84c93-54ef-463f-a7ef-ae25ff887087
  6     0x00002822      0x00004821      "fip-b"
        attrs:  0x0000000000000000
        type:   19d5df83-11b0-457b-be2c-7559c13142a5
                (19d5df83-11b0-457b-be2c-7559c13142a5)
        guid:   09c54952-d5bf-45af-acee-335303766fb3
  7     0x00004822      0x00004c21      "u-boot-env"
        attrs:  0x0000000000000000
        type:   3de21764-95bd-54bd-a5c3-4abe786f38a8
                (u-boot-env)
        guid:   2e4a1b1e-a2ac-4420-9de8-5076a7a68710
  8     0x00004c22      0x00404c21      "bootfs"
        attrs:  0x0000000000000004
        type:   0fc63daf-8483-4772-8e79-3d69d8477de4
                (linux)
        guid:   df8b96f1-b7b6-4620-b470-0a828414ac29
  9     0x00404c22      0x0eebffde      "userfs"
        attrs:  0x0000000000000000
        type:   0fc63daf-8483-4772-8e79-3d69d8477de4
                (linux)
        guid:   6d7aa1d7-4404-4d29-af0f-1ce7f5379a29
```

following files does boot partition from yocto delivery has:

```sh
STM32MP> ext4ls mmc 0:8
STM32MP> fatls mmc 0:8
            devicetree/
            mmc0_extlinux/
            mmc1_extlinux/
    90867   stm32mp215f-dk.dtb
    90863   stm32mp215f-dk-psci-osi.dtb
   119359   stm32mp257f-dk.dtb
   118218   stm32mp257f-ev1.dtb
   118194   stm32mp257f-ev1-psci-osi.dtb

5 file(s), 3 dir(s)

STM32MP> fatls mmc 0:8 /devicetree
            ./
            ../
     1039   stm32mp215f-dk-m2-bcm43xx-2ae.dtbo
     1039   stm32mp215f-dk-m2-bcm43xx-2bz.dtbo

2 file(s), 2 dir(s)

STM32MP> fatls mmc 0:8 /mmc0_extlinux
            ./
            ../
      264   extlinux.conf
      802   stm32mp215f-dk_extlinux.conf
      554   stm32mp257f-ev1_extlinux.conf

3 file(s), 2 dir(s)

STM32MP> fatls mmc 0:8 /mmc1_extlinux
            ./
            ../
      264   extlinux.conf
      554   stm32mp257f-ev1_extlinux.conf

2 file(s), 2 dir(s)

```

U-Boot itself doesn't do mkfs — do this from Linux (host PC or the STM32MP's own running Linux, if it has access to /dev/sdc8):

```sh
sudo mkfs.vfat -F 32 -n QNX /dev/sdc8
```

Transfer directly to u-boot mmc partition (or put your mmc card into mmc card reader):

on u-boot client run:

```sh
ums 0 mmc 0
```

Connect appearing usb drive to your host.

(adjust device node to match your actual SD/eMMC — check with `lsblk`)

Then mount and copy your IFS image:

```sh
sudo mount /dev/sdc8 /mnt
sudo cp ifs-rpi5.raw /mnt/     # or your STM32MP2-built IFS image
sudo umount /mnt
```

2. Verify from U-Boot

```sh
STM32MP> fatls mmc 0:8
```

You should see your .raw file listed.

3. Set U-Boot env vars to load and boot it

```sh
setenv ifs_file ifs-stm32mp257f-dk.raw
setenv ifs_part 8
# setenv fdtfile stm32mp257f-dk.dtb
setenv load_dtb 'fatload mmc 0:${ifs_part} ${fdt_addr_r} ${fdtfile}'
setenv loadifs 'fatload mmc 0:${ifs_part} ${kernel_addr_r} ${ifs_file}'
setenv bootifs 'run load_dtb; run loadifs; go ${kernel_addr_r} ${fdt_addr_r}'
setenv loadtftp 'tftp ${kernel_addr_r} ${ifs_file}'
setenv boottftp 'run load_dtb; run loadtftp; go ${kernel_addr_r} ${fdt_addr_r}'
saveenv
```

This reuses `kernel_addr_r=0x8a000000` already defined in your environment as the load address.

1. Boot it

```sh
run bootifs
```

5. Setup tftp server connection in U-Boot:

```sh
setenv ipaddr 192.168.1.101
setenv serverip 192.168.1.1
setenv gatewayip 192.168.1.1
setenv netmask 255.255.255.0
saveenv
```

6. Use tftp boot:

```sh
run boottftp
```

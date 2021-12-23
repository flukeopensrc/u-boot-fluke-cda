/*
 *  Copyright Altera Corporation (C) 2012-2013. All rights reserved
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms and conditions of the GNU General Public License,
 *  version 2, as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CONFIG_SOCFPGA_FLUKE_CDA_NIGHTHAWK_H__
#define __CONFIG_SOCFPGA_FLUKE_CDA_NIGHTHAWK_H__

#include <asm/arch/base_addr_ac5.h>
/*
 * Console setup
 */
/* Monitor Command Prompt */
/*
#define CONFIG_SYS_PROMPT		"Fluke Nighthawk A9 Rev002 # " 
*/

/* Define machine type for Cyclone 5 */
#define CONFIG_MACH_TYPE 4251

/* U-Boot Commands */
#define CONFIG_SYS_NO_FLASH
#define CONFIG_DOS_PARTITION
#define CONFIG_FAT_WRITE
/* Do NOT set CONFIG_HW_WATCHDOG, it will result in the hardware reset for the watchdog 
 * never being asserted (see the code for arch_cpu_init() in arch/arm/mach-socfpga/misc.c
 * if you don't believe me).  This results in odd behavior, like Linux spontaneously 
 * rebooting a while after being shut down.
 */
/* #define CONFIG_HW_WATCHDOG 
 */

/* Memory configurations */
#define PHYS_SDRAM_1_SIZE		0x40000000
#define CONFIG_CMD_SDRAM

/* Booting Linux */
#define CONFIG_BOOTFILE         "zImage"
#define CONFIG_BOOTARGS         "console=ttyS0," __stringify(CONFIG_BAUDRATE)
#ifdef CONFIG_SOCFPGA_VIRTUAL_TARGET
#define CONFIG_BOOTCOMMAND      "run ramboot"
#else

#define CONFIG_BOOTCOMMAND      "run qspifpga; bridge enable; run qspiload; run qspiboot"

#endif
#define CONFIG_LOADADDR         0x1000000
#define CONFIG_SYS_LOAD_ADDR    CONFIG_LOADADDR

/* Ethernet on SoC (EMAC) */
#if defined(CONFIG_CMD_NET)
#define CONFIG_PHY_MICREL
#define CONFIG_PHY_MICREL_KSZ9021
#endif

/* enable date env command */
#define CONFIG_CMD_DATE
#define CONFIG_RTC_PCF8563
#define CONFIG_SYS_RTC_BUS_NUM 1
#define CONFIG_SYS_I2C_RTC_ADDR 0X51

/* environment setting for MMC */
#define CONFIG_ENV_IS_IN_SPI_FLASH

/* support for booting recue kernel with initrd */
#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_INITRD_TAG

/* Enabled gzwrite command for use in writing the initial partitioned disk 
 * image containing the root fs.  They are highly compressible so this lets us
 * initialize the eMMC much more quickly. */ 
#define CONFIG_CMD_UNZIP

/* Extra Environment
 * 
 * initrd_high must be set for rescue kernel boot, since otherwise it defaults to 0x400000 (64M)
 * which does not allow enough free space for the initrd image, and u-boot will have an allocation
 * error when bootz is run.  It also must not be set above 0x30000000 or it will prevent linux
 * from booting.
 *
 * We are using the "backup" kernel for the rescue boot ("backup" meaning the kernel
 * which can be copied over the production kernel when a rescue restore is performed).  We may
 * at some point want to use a seperate kernel compiled with different kernel options as the
 * rescue kernel, or a simplified device tree, but we are keeping it simple for now.
 *
 * Note we are currently use the production fpga image always, which could potentially
 * be an issue since the eMMC requires the fpga to be programmed in order to operate.
 * We could be more sophisticated about handling this if the fpga loading were moved
 * from u-boot into the rescue kernel.
 * 
 * The "backup" kernel/u-boot/device trees/fpga image/etc are offset by 0x2000000 (32M) in the qspi from 
 * the primary ones.
 */
#define CONFIG_EXTRA_ENV_SETTINGS \
        "verify=n\0" \
        "autoload=no\0" \
        "myserverip=10.0.0.10\0" \
        "loadaddr=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
        "ethaddr=00:80:40:ff:00:17\0" \
        "scriptfile=u-boot.scr\0" \
        "stderr=serial\0" \
        "stdin=serial\0" \
        "stdout=serial\0" \
        "bootimage=zImage\0" \
        "bootimagesize=0x700000\0" \
        "fdtaddr=100\0" \
        "fdtimage=Blackhawk_soc.dtb\0" \
        "fdtimagesize=0x7F00\0" \
        "fpga=0\0" \
        "fpgadata=0x2000000\0" \
        "fpgadatasize=0x600000\0" \
        "fpgadata=0x2000000\0" \
        "qspifdtaddr=0x0C0000\0" \
        "qspibootimageaddr=0x100000\0" \
        "qspiloadcs=0\0" \
        "qspifpgaaddr=0x3800000\0" \
        "qspisplashimageaddr=0x3E00000\0" \
        "qspiroot=/dev/mtdblock4\0" \
        "qspirootfstype=jffs2\0" \
        "qspifpga=sf probe ${qspiloadcs};" \
                " sf read ${fpgadata} ${qspifpgaaddr} ${fpgadatasize};" \
                " fpga load ${fpga} ${fpgadata} ${fpgadatasize}\0" \
        "qspiload=sf probe ${qspiloadcs};sf read ${loadaddr} ${qspibootimageaddr} ${bootimagesize};" \
                  "sf read ${fdtaddr} ${qspifdtaddr} ${fdtimagesize};\0" \
        "qspiboot=setenv bootargs console=ttyS0," __stringify(CONFIG_BAUDRATE) \
                  " root=${qspiroot} rw rootfstype=${qspirootfstype} coherent_pool=1M;" \
                  " bootz ${loadaddr} - ${fdtaddr}\0" \
        "splashloadaddr=0xC100000\0" \
        "splashimagesize=0x130000\0" \
        "splashload=sf probe ${qspiloadcs}; sf read ${splashloadaddr} ${qspisplashimageaddr} ${splashimagesize}\0" \
        "splashboot=run qspifpga; bridge enable; run splashload; go 0xC1000D9\0" \
        "netboot=dhcp; setenv serverip ${myserverip};" \
               " tftp ${serverip}:${bootimage};" \
               " tftp ${fdtaddr} ${serverip}:${fdtimage};" \
               " setenv bootargs console=ttyS0," __stringify(CONFIG_BAUDRATE) \
               " root=${qspiroot} rw rootfstype=${qspirootfstype} coherent_pool=1M;" \
               " bootz ${loadaddr} - ${fdtaddr}\0"

#include "socfpga_common.h"

#endif  /* __CONFIG_SOCFPGA_FLUKE_CDA_NIGHTHAWK_H__ */

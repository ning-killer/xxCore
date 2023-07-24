#include <command.h>
#include <common.h>
#include <asm/io.h>
#include <imgs.h>
#include <ubifs_uboot.h>
#ifdef CONFIG_NAND_SUPPORT
#include <nand.h>
#endif
#include "cvi_update.h"

#define COMPARE_STRING_LEN 3
#define SD_UPDATE_MAGIC 0x4D474E32
#define ETH_UPDATE_MAGIC 0x4D474E35
#define USB_DRIVE_UPGRADE_MAGIC 0x55425355
#define FIP_UPDATE_MAGIC 0x55464950
#define UPDATE_DONE_MAGIC 0x50524F47
#define OTA_MAGIC 0x5245434F
//#define ALWAYS_USB_DRVIVE_UPGRATE
#define HEADER_SIZE 64
#define HEADER_MAGIC "CIMG"
#define MAX_LOADSIZE 100 * 1024 * 1024
#ifdef CONFIG_CMD_SAVEENV
#define SET_DL_COMPLETE()                                                 \
	setenv("dl_flag", "prog");                                            \
	run_command("saveenv", 0)
#else
#define SET_DL_COMPLETE() writel(0x50524F47, UPGRADE_SRAM_ADDR)
#endif /* CONFIG_CMD_SAVEENV */

#ifdef CONFIG_NAND_SUPPORT
static uint32_t lastend;
#endif

enum chunk_type_e { dont_care = 0, check_crc };
enum storage_type_e { sd_dl = 0, usb_dl };

static int _storage_update(enum storage_type_e type);
static int _usb_update(uint32_t usb_pid);
static int _eth_upgrade(void);

static uint32_t bcd2hex4(uint32_t bcd)
{
	return ((bcd) & 0x0f) + (((bcd) >> 4) & 0xf0) + (((bcd) >> 8) & 0xf00) + (((bcd) >> 12) & 0xf000);
}

int _prgImage(char *file, uint32_t chunk_header_size)
{
	uint32_t chunk_type = *(uint32_t *)((uintptr_t)file);
	uint32_t size = *(uint32_t *)((uintptr_t)file + 4);
	uint32_t offset = *(uint32_t *)((uintptr_t)file + 8);
	uint32_t header_crc = *(uint32_t *)((uintptr_t)file + 12);
	char cmd[255] = { '\0' };
	int ret = 0;

	//if (chunk_type == check_crc) {
	//	uint32_t crc = crc32(
	//		0, (unsigned char *)file + chunk_header_size, size);
	//	if (crc != header_crc) {
	//		printf("Crc check failed header(0x%08x) img(0x%08x), skip it\n",
	//		       header_crc, crc);
	//		return 0;
	//	} else {
	//		/* Invalidate crc to avoid program garbage */
	//		*(uint32_t *)((uintptr_t)file + 12) = 0;
	//	}
	//}
#ifdef CONFIG_NAND_SUPPORT
	int dev = nand_curr_device;
	struct mtd_info *mtd = nand_info[dev];
	int goodblocks = 0;

	// Calculate real offset when programming chunk.
	if (offset < lastend)
		offset = lastend;
	else
		lastend = offset;
	for (; goodblocks * mtd->erasesize < size; lastend += mtd->erasesize) {
		if (!nand_block_isbad(mtd, lastend))
			goodblocks++;
	}
	lastend += mtd->erasesize;
	pr_debug("offset:0x%x lastoffset:0x%x\n", offset, lastend);

	snprintf(cmd, 255, "nand write %p 0x%x 0x%x",
		 (void *)file + chunk_header_size, offset, size);
#elif defined(CONFIG_SPI_FLASH)
	snprintf(cmd, 255, "sf update %p 0x%x 0x%x",
		 (void *)file + chunk_header_size, offset, size);
#else
	run_command("mmc dev 0 0", 0);
	snprintf(cmd, 255, "mmc write %p 0x%x 0x%x",
		 (void *)file + chunk_header_size, offset, size / 0x200 + 1);
#endif
	pr_debug(cmd);
	ret = run_command(cmd, 0);
	if (ret)
		return 0;

	return size;
}

static int _checkHeader(char *file, char strStorage[10])
{
	char *magic = (void *)HEADER_ADDR;
	uint32_t version = *(uint32_t *)((uintptr_t)HEADER_ADDR + 4);
	uint32_t chunk_sz = *(uint32_t *)((uintptr_t)HEADER_ADDR + 8);
	uint32_t total_chunk = *(uint32_t *)((uintptr_t)HEADER_ADDR + 12);
	uint32_t file_sz = *(uint32_t *)((uintptr_t)HEADER_ADDR + 16);
#ifdef CONFIG_NAND_SUPPORT
	char *extra = (void *)((uintptr_t)HEADER_ADDR + 20);
	static char prevExtra[EXTRA_FLAG_SIZE + 1] = { '\0' };
#endif
	int ret = strncmp(magic, HEADER_MAGIC, 4);

	if (ret) {
		printf("File:%s Magic number is wrong, skip it\n", file);
		return ret;
	}
	printf("Header Version:%d\n", version);
	char cmd[255] = { '\0' };
	uint32_t pos = HEADER_SIZE;
#ifdef CONFIG_NAND_SUPPORT
	// Erase partition first
	if (strncmp(extra, prevExtra, EXTRA_FLAG_SIZE)) {
		strncpy(prevExtra, extra, EXTRA_FLAG_SIZE);
		snprintf(cmd, 255, "nand erase.part -y %s", prevExtra);
		pr_debug("%s\n", cmd);
		run_command(cmd, 0);
	}
	lastend = 0;
#endif
	for (int i = 0; i < total_chunk; i++) {
		uint32_t load_size = file_sz > (MAX_LOADSIZE + chunk_sz) ?
						   MAX_LOADSIZE + chunk_sz :
						   file_sz;
		snprintf(cmd, 255, "fatload %s %p %s 0x%x 0x%x;", strStorage,
			 (void *)IMG_ADDR, file, load_size, pos);
		pr_debug("%s\n", cmd);
		ret = run_command(cmd, 0);
		if (ret)
			return ret;

		ret = _prgImage((void *)IMG_ADDR, chunk_sz);
		if (ret == 0) {
			printf("program file:%s failed\n", file);
			break;
		}
		pos += load_size;
		file_sz -= load_size;
	}
	return 0;
}

static int _storage_update(enum storage_type_e type)
{
	int ret = 0;
	char cmd[255] = { '\0' };
	char strStorage[10] = { '\0' };

	if (type == sd_dl) {
		strncpy(strStorage, "mmc 1:1", 9);
		printf("Start SD downloading...");
	} else {
#if defined(CONFIG_CMD_USB) && defined(CONFIG_USB_STORAGE)
		strncpy(strStorage, "usb 0", 9);
		printf("Start USB downloading...");
#if defined(CV1835_WEVB_0001A) || defined(CV1835_WEVB_0002A) ||                              \
	defined(CV1835_WEVB_0002A_DDR3X16) || defined(CV1835_WEVB_0004A_SPINAND)
		// Enable USB hub power
		writel(0x03800000, 0x03020000); //mw 0x03020000 0x03800000 1
		writel(0x03800000, 0x03020004); //mw 0x03020004 0x03800000 1
		writel(0x08100000, 0x03021000); //mw 0x03021000 0x08100000 1
		writel(0x08100000, 0x03021004); //mw 0x03021004 0x08100000 1
		writel(0x0, 0x03020000); //mw 0x03020000 0x00000000 1
		writel(0x03800000, 0x03020000); //mw 0x03020000 0x03800000 1
#endif
		ret = run_command("usb start", 0);
#else
		printf("Please enable CONFIG_CMD_USB and CONFIG_USB_STORAGE for usb drive upgrade\n");
		return 1;
#endif
	}
	// Only update when sd update
	if (type == 0) {
		snprintf(cmd, 255, "fatload %s %p fip.bin;", strStorage,
			 (void *)HEADER_ADDR);
		ret = run_command(cmd, 0);
#ifdef CONFIG_NAND_SUPPORT
		snprintf(cmd, 255, "cvi_sd_update %p spinand fip",
			 (void *)HEADER_ADDR);
		ret = run_command(cmd, 0);
#elif defined(CONFIG_SPI_FLASH)
		run_command("sf probe", 0);
		snprintf(cmd, 255,
			 "sf update %p ${fip_PART_OFFSET} ${filesize};",
			 (void *)HEADER_ADDR);
		ret = run_command(cmd, 0);
#else
		run_command("mmc dev 0 1", 0);
		snprintf(cmd, 255, "mmc write %p 0 0x800;",
			 (void *)HEADER_ADDR);
		run_command(cmd, 0);
		snprintf(cmd, 255, "mmc write %p 0x800 0x800;;",
			 (void *)HEADER_ADDR);
		run_command(cmd, 0);
		printf("Program fip.bin done");
#endif
	}
	for (int i = 1; i < ARRAY_SIZE(imgs); i++) {
		snprintf(cmd, 255, "fatload %s %p %s 0x%x 0;", strStorage,
			 (void *)HEADER_ADDR, imgs[i], HEADER_SIZE);
		pr_debug("%s\n", cmd);
		ret = run_command(cmd, 0);
		if (ret) {
			printf("load %s failed, skip it!\n", imgs[i]);
			continue;
		}
		if (_checkHeader(imgs[i], strStorage))
			continue;
	}
	if (ret == 0)
		SET_DL_COMPLETE();
	return 0;
}

static int _usb_update(uint32_t usb_pid)
{
	int ret = 0;
	char cmd[255] = { '\0' };
	char utask_cmd[255] = { '\0' };
#ifdef CONFIG_NAND_SUPPORT
	static char prevExtra[EXTRA_FLAG_SIZE + 1] = { '\0' };
#endif
	printf("Start USB downloading...");

	// Clean download flags
	writel(0x0, USB_UPDATE_SRAM_ADDR); //mw.l 0xe00fc00 0x0;
	// Always download Fip first
	snprintf(utask_cmd, 255, "cvi_utask vid 0x3346 pid 0x%x", usb_pid);
	ret = run_command(utask_cmd, 0);
#ifdef CONFIG_NAND_SUPPORT
	snprintf(cmd, 255, "cvi_sd_update %p spinand fip", (void *)IMG_ADDR);
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
#elif defined(CONFIG_SPI_FLASH)
	ret = run_command("sf probe", 0);
	snprintf(cmd, 255, "sf update %p ${fip_PART_OFFSET} ${fip_PART_SIZE};", (void *)IMG_ADDR)
	pr_debug("%s\n", cmd);
	ret = run_command(cmd, 0);
#else
	run_command("mmc dev 0 1", 0);
	snprintf(cmd, 255, "mmc write %p 0 0x800;", (void *)IMG_ADDR);
	pr_debug("%s\n", cmd);
	run_command(cmd, 0);
	snprintf(cmd, 255, "mmc write %p 0x800 0x800;", (void *)IMG_ADDR);
	pr_debug("%s\n", cmd);
	run_command(cmd, 0);
	printf("Program fip.bin done");
#endif
	// Since device will reset by host tool, set flag first
	SET_DL_COMPLETE();
	while (1) {
		ret = run_command(utask_cmd, 0);
		if (ret) {
			error("cvi_utask failed(%d)\n", ret);
			return ret;
		}
		//_prgImage((void *)IMG_ADDR, readl(HEADER_ADDR + 8));
	};
	return 0;
}

static int _eth_upgrade(void)
{
#ifdef CONFIG_FASTBOOT
	uint32_t *const ip_ptr = (void *)ETH_UPGRADE_IP_ADDR;
	uint32_t ip = 0;

	printf("Start Ethernet upgrading...");
	writel(0x0, UPGRADE_SRAM_ADDR);
	ip = *ip_ptr;
	memcpy(&net_ip, &ip, 4);
	printf("Do Ethernet update. IP:%pI4\n", &net_ip);
	run_command("fastboot udp", 0);
	return 0;
#else
	printf("Please enable Fastboot for ethernet upgrade\n");
	return 1;
#endif
}

#define FIP_PATH ".ota/fip.bin" // Which means /mnt/data/.ota/fip.bin
static void _update_fip_from_data(void)
{
#ifdef CONFIG_NAND_SUPPORT
	char cmd[255] = { '\0' };
	loff_t actread;
	int ret = 0;
	/* Mounting Data parition */
	writel(UPDATE_DONE_MAGIC, UPGRADE_SRAM_ADDR); // Clean sram flag fisrt.
	ret = run_command("ubi part DATA", 0);
	if (ret) {
		printf("Ubi attach Data parition failed! Exit update fip procedure.\n");
		return;
	}
	ret = run_command("ubifsmount ubi0:DATA", 0);
	if (ret) {
		printf("Mount ubi data parition failed!! Exit update fip procedure.\n");
		return;
	}
	/* Load fip to IMG_ADDR */
	ret = ubifs_read(FIP_PATH, (void *)IMG_ADDR, 0, 0, &actread);
	if (ret) {
		printf("Load %s failed!! Exit update fip procedure.\n",
		       FIP_PATH);
		return;
	}
	snprintf(cmd, 255, "cvi_sd_update %p spinand fip", (void *)IMG_ADDR);
	run_command(cmd, 0);
	run_command("reset", 0);
#endif
}

static int do_cvi_update(cmd_tbl_t *cmdtp, int flag, int argc,
			 char *const argv[])
{
    return ep_check_cvi_update(0,0);
}
static int do_cvi_update2(cmd_tbl_t *cmdtp, int flag, int argc,
			 char *const argv[])
{
	int ret = 1;
	char args[255] = { '\0' };
	uint32_t usb_pid = 0;

	if (argc == 1) {
		if (readl(SD_UPDATE_SRAM_ADDR) == SD_UPDATE_MAGIC) {
			run_command("env default -a", 0);
			ret = _storage_update(sd_dl);
		} else if (readl(USB_UPDATE_SRAM_ADDR) == USB_UPDATE_MAGIC) {
			run_command("env default -a", 0);
			usb_pid = in_be32(UBOOT_PID_SRAM_ADDR);
			usb_pid = bcd2hex4(usb_pid);
			ret = _usb_update(usb_pid);
		} else {
			uint32_t magic = readl(UPGRADE_SRAM_ADDR);
#ifndef ALWAYS_USB_DRVIVE_UPGRATE
			switch (magic) {
			case ETH_UPDATE_MAGIC:
				ret = _eth_upgrade();
				break;
			case USB_DRIVE_UPGRADE_MAGIC:
				ret = _storage_update(usb_dl);
				break;
			case FIP_UPDATE_MAGIC:
				_update_fip_from_data();
			case OTA_MAGIC:
				snprintf(args, 255, "rdinit=/init_recovery");
				setenv("bootargs_emmc", args);
				/* Set data partition */
				snprintf(args, 255,
					 MTDPARTS_DEFAULT
					 " ubi.mtd=DATA rdinit=/init_recovery");
				setenv("bootargs_nand", args);
				printf("Entering recovery mode\n");
				break;
			}
#else
			ret = _storage_update(usb_dl);
			if (ret && magic == ETH_UPDATE_MAGIC)
				ret = _eth_upgrade();
#endif
		}

	} else {
		printf("Usage:\n%s\n", cmdtp->usage);
	}

	return ret;
}

U_BOOT_CMD(
	cvi_update, 2, 0, do_cvi_update,
	"cvi_update [eth, sd, usb]- check boot status and update if necessary\n",
	"run cvi_update without parameter will check the boot status and try to update");

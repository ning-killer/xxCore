/*
 * (C) Copyright 2002-2008
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#define CONFIG_FILE "env.cfg"
#define DEFAULT_FILE "default"

#ifndef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE		115200
#endif

#ifndef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	3	/* autoboot after 5 seconds	*/
#endif

#ifndef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND						\
	"bootp; "							\
	"setenv bootargs root=/dev/nfs nfsroot=${serverip}:${rootpath} "\
	"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}::off; "\
	"bootm"
#endif

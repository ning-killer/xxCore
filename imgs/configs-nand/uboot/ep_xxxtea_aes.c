//
// Created by tfh on 2021/10/20.
//

#include <common.h>
#include <command.h>
#include <environment.h>
#include <uboot_aes.h>
#include <malloc.h>
#include <asm/byteorder.h>
#include <linux/compiler.h>

#include <u-boot/md5.h>
#include <asm/io.h>
/*aes_xxxtea 是密文，解密后前16个值是aes key,后16 个值是iv
 * 解密内容是:eapilaesfirmwareFIRMWAREAESEAPIL*/
static uint32_t aes_xxxtea[32] = {
        0xd714d761, 0x21ff4135, 0x75c4e4b0, 0xcd84bb5c, 0x4accd126, 0xb4ffe21d, 0xb7b626d6, 0x962fc5d6,
        0x8af3783e, 0xaf733192, 0xc550fa4b, 0xec000dc7, 0xb03c39f5, 0xc0b07ecf, 0x4bed8b27, 0xde77e493,
        0xeb1bbfc4, 0x54b739d9, 0x4ade5306, 0x78a90091, 0xa312b3c2, 0x731d6b91, 0x432e3690, 0xdafebc2f,
        0x7a0681db, 0xb008568f, 0x60407de3, 0x486729ba, 0x6241046e, 0x27d8f63b, 0x325f12da, 0x237c71b6
};
#define DE_EAPIL_TYPE "eapil_rom_enc"
static uint32_t xxxtea_key[4] = {0x45, 0x41, 0x50,0x494C};//代表 EAPIL 0x494c="IL"
static unsigned char g_aes_user_key[16];
static unsigned char g_aes_user_iv[16];
static unsigned int g_aes_user_key_len = 128;

static unsigned char  g_aes_key[AES_EXPAND_KEY_LENGTH];

typedef struct {
    unsigned int data_size; //整个数据包的大小 flash_size+sizeof(aes_encryt_head_t)
    unsigned int flash_size;  //flash大小
    unsigned int kernel_size;  //kernel大小
    unsigned int rootfs_size;  //rootfs大小
    unsigned int fill_size; //aes加密16字节对其后需要填充的字节
    unsigned char flash_md5[36];  //flash md5值
    unsigned char kernel_md5[36];  //内核md5值
    unsigned char rootfs_md5[36];  //文件系统md5值
    char eapil_type[16];    //专用验证包
}aes_encryt_head_t; //144 Byte

#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

static unsigned int xxxtea_InitKey(){
    int i;
    uint32_t result;
    uint8_t t[]={0xBC,0x15,0x5B,0x9B};
    uint32_t table_key = 0xdeadbeef;

    uint8_t k1 = table_key & 0xff,
            k2 = (table_key >> 8) & 0xff,
            k3 = (table_key >> 16) & 0xff,
            k4 = (table_key >> 24) & 0xff;

    for(i=0; i<4; i++){
        t[i] ^= k1; t[i] ^= k2; t[i] ^= k3; t[i] ^= k4;
    }

    result = t[3];
    result |= t[2]<<8;
    result |= t[1]<<16;
    result |= t[0]<<24;

    return result;
}
/*使用xxxtea 魔改算法生成aes的key和iv
 * n 大于0代表加密，n<0 代表解密
 * */
static void xxxtea_btea(uint32_t *v, int n, uint32_t const key[4])
{
    uint32_t y, z, sum, delta;
    unsigned p, rounds, e;

    delta=xxxtea_InitKey();
    if (n > 1)            /* Coding Part */
    {
        rounds = 6 + 52/n;
        sum = 0;
        z = v[n-1];
        do
        {
            sum += delta;
            e = (sum >> 2) & 3;
            for (p=0; p<n-1; p++)
            {
                y = v[p+1];
                z = v[p] += MX;
            }
            y = v[0];
            z = v[n-1] += MX;
        }
        while (--rounds);
    }
    else if (n < -1)      /* Decoding Part */
    {
        n = -n;
        rounds = 6 + 52/n;
        sum = rounds*delta;
        y = v[0];
        do
        {
            e = (sum >> 2) & 3;
            for (p=n-1; p>0; p--)
            {
                z = v[p-1];
                y = v[p] -= MX;
            }
            z = v[n-1];
            y = v[0] -= MX;
            sum -= delta;
        }
        while (--rounds);
    }
}

static void ep_xxxtea_aes_init()
{
    int i = 0;
    xxxtea_btea(aes_xxxtea,-32,xxxtea_key);
    for(i = 0;i < 16;i++){
        g_aes_user_key[i] = aes_xxxtea[i]&0xff;
    }

    for(i = 16;i < 32;i++){
        int iv_i = (i-16);
        g_aes_user_iv[iv_i] = aes_xxxtea[i]&0xff;
    }

    xxxtea_btea(aes_xxxtea,32,xxxtea_key);

    /* First we expand the key. */
	aes_expand_key(g_aes_user_key, g_aes_key);

}

static void aes_cbc_decrypt_blocks2(u8 *key_exp, u8 *src, u8 *dst, u32 num_aes_blocks,u8 *cbc_chain_data)
{
    u8 tmp_data[AES_KEY_LENGTH], tmp_block[AES_KEY_LENGTH];
    /* Convenient array of 0's for IV */
    //u8 cbc_chain_data[AES_KEY_LENGTH] = { 0 };
    u32 i;

    for (i = 0; i < num_aes_blocks; i++) {
        debug("encrypt_object: block %d of %d\n", i, num_aes_blocks);

        memcpy(tmp_block, src, AES_KEY_LENGTH);

        /* Decrypt the AES block */
        aes_decrypt(src, key_exp, tmp_data);

        /* Apply the chain data */
        aes_apply_cbc_chain_data(cbc_chain_data, tmp_data, dst);

        /* Update pointers for next loop. */
        memcpy(cbc_chain_data, tmp_block, AES_KEY_LENGTH);
        src += AES_KEY_LENGTH;
        dst += AES_KEY_LENGTH;
    }
}


static void ep_xxxtea_aes_decrypt(char *in_data, int size,char *out_data)
{
    /* Calculate the number of AES blocks to encrypt. */
    unsigned int aes_blocks = DIV_ROUND_UP(size, AES_KEY_LENGTH);
    aes_cbc_decrypt_blocks2(g_aes_key,in_data,out_data,aes_blocks,g_aes_user_iv);
}
/*解密epflash文件，并保存问题信息*/
int ep_xxxtea_aes_decrypt_check(char *src_addr,int size)
{
    aes_encryt_head_t aes_head = {0};
    int ret = 0;
    int i;
    char *pdata = src_addr;
    char *dst_addr = src_addr;
    ep_xxxtea_aes_init();
    ep_xxxtea_aes_decrypt(pdata, sizeof(aes_encryt_head_t), (char *)&aes_head);
    int need_len = aes_head.flash_size;
    printf("data_size = %d\n",aes_head.data_size); //整个数据包的大小 flash_size+sizeof(aes_encryt_head_t)
    printf("flash_size = %d\n",aes_head.flash_size);  //固件大小
    printf("fill_size = %d\n",aes_head.fill_size); //aes加密16字节对其后需要填充的字节
    printf("flash_md5[] = %s\n",aes_head.flash_md5);  //固件md5值
    printf("eapil_type[] = %s\n",aes_head.eapil_type);

    if((need_len < 0) ||(need_len > size)){
        printf("needlen = %d,size=%d\n",need_len,size);
        return -1;
    }
    if(strncmp(aes_head.eapil_type,DE_EAPIL_TYPE,strlen(DE_EAPIL_TYPE)) != 0){
        printf("eapil_type = %s,err\n",aes_head.eapil_type);
        return -1;
    }
    struct MD5Context ctx;
    unsigned char temp_md5[17] = {0};
    char calculate_md5[36] = {0};
    MD5Init(&ctx);

    pdata += sizeof(aes_encryt_head_t);
    while(need_len > 0) {
        int writelen = 16;
        char pdata2[32] = {0};
        /*解密*/
        ep_xxxtea_aes_decrypt(pdata, writelen, dst_addr);
        if(need_len <= writelen){
            writelen -= aes_head.fill_size;
        }
        need_len -= 16;
        /*md5校验*/
        MD5Update(&ctx, dst_addr, writelen);
        pdata += writelen;
        dst_addr += writelen;
    }
    MD5Final(temp_md5, &ctx);
    for(i=0; i<16; i++) {
        sprintf(calculate_md5+i*2, "%02x", temp_md5[i]);
    }
    printf("read md5: %s\n", aes_head.flash_md5);
    printf( "calculate md5: %s\n", calculate_md5);
    if (strncmp(aes_head.flash_md5, calculate_md5,32) != 0) {
        printf( "incomplete rom\n");
        return -1;
    }
    char *env_md5 = getenv("flash_md5");
    if(env_md5){
        if (strncmp(env_md5, calculate_md5,32) == 0) {
            printf( "flash data same ,Suggestion not update!\n");
           // return -2;
        }
    }
    setenv("flash_md5",aes_head.flash_md5);
    setenv("kernel_md5",aes_head.kernel_md5);
    setenv("rootfs_md5",aes_head.rootfs_md5);
    setenv_hex("filesize",aes_head.flash_size);
    setenv_hex("kernel_size",aes_head.kernel_size);
    setenv_hex("rootfs_size",aes_head.rootfs_size);
    return 0;
}
 
static int img_read_mem_map_2md5( int map_len,int map_offset,char *src_md5,char *md5_name)
{
    if((map_len < 1) || (map_offset < 1)){
        printf("img_read_mem_map_2md5 err! ,map_len=%d,map_offset=%d,src_md5=%s \n",map_len,map_offset);
        return -1;
    }
    int ret = 0;
    char cmd[256] = { '\0' };
    sprintf(cmd, "sf read %#x %#x %#x",HEADER_ADDR,map_offset,map_len);
    ret = run_command(cmd, 0);
    if(ret != 0){
        printf("read kernel img err! \n");
        return -1;
    }
    char *buf = map_physmem((unsigned long) HEADER_ADDR, map_len, MAP_WRBACK);
    if (buf) {
        printf("map_physmem  ok,start check! map_len=%#x \n", map_len);
        int i = 0;
        unsigned char temp_md5[17] = {0};
        char calculate_md5[36] = {0};
        md5(buf, map_len, temp_md5);
        for (i = 0; i < 16; i++) {
            sprintf(calculate_md5 + i * 2, "%02x", temp_md5[i]);
        }
        unmap_physmem(buf, map_len);
		if(src_md5 == NULL){
   	 		setenv(md5_name,calculate_md5);
			printf("Save the %s validation for the first time!\n",md5_name);
		}else{
	        printf("src_md5 md5: %s\n", src_md5);
	        printf("calculate md5: %s\n", calculate_md5);
	        if (strncmp(src_md5, calculate_md5, 32) != 0) {
	            printf("incomplete img ,Stop starting!\n");
	            return -1;
	        }
		}
        return 0;
    }
    printf("map_physmem err!\n");

    return -1;
}

static int kernel_md5_check()
{
    int map_len = getenv_hex("kernel_size",0);	
	if(map_len < 1){
		map_len = getenv_hex("BOOT_PART_SIZE",0);
	}
    int map_offset = getenv_hex("BOOT_PART_OFFSET",0);
    char *env_md5 = getenv("kernel_md5");
    return img_read_mem_map_2md5(map_len,map_offset,env_md5,"kernel_md5");
}
static int rootfs_md5_check()
{
    int map_len = getenv_hex("rootfs_size",0);
	if(map_len < 1){
		map_len = getenv_hex("ROOTFS_PART_SIZE",0);
	}
    int map_offset = getenv_hex("ROOTFS_PART_OFFSET",0);
    char *env_md5 = getenv("rootfs_md5");
    return img_read_mem_map_2md5(map_len,map_offset,env_md5,"rootfs_md5");
}
/*读取默认配置环境变量*/
static int read_default_flash_env()
{
	char *env_flash_md5 = getenv("flash_md5");
    if(env_flash_md5){
		return 0;
	}  
	printf("env_flash is NULL,read env default!\n");
   	run_command("sf probe", 0);
	
    int map_len = sizeof(aes_encryt_head_t);
    int conf_offset = getenv_hex("CONFIGS_PART_OFFSET",0);
    int conf_offlen = getenv_hex("CONFIGS_PART_SIZE",0);
	int map_offset = conf_offset+conf_offlen-1024;
	int ret = 0;
    char cmd[256] = { '\0' };
    sprintf(cmd, "sf read %#x %#x %#x",HEADER_ADDR,map_offset,map_len);
    ret = run_command(cmd, 0);
    if(ret != 0){
        printf("read_default_flash_env img err! \n");
        return -1;
    }
    char *buf = map_physmem((unsigned long) HEADER_ADDR, map_len, MAP_WRBACK);
    if (buf) {
        printf("map_physmem  ok,start check! map_len=%#x \n", map_len);
		aes_encryt_head_t head;
		memcpy(&head,buf,map_len);
		memset(buf,0xff,map_len);
        unmap_physmem(buf, map_len);
		
		setenv("flash_md5",head.flash_md5);
	    setenv("kernel_md5",head.kernel_md5);
	    setenv("rootfs_md5",head.rootfs_md5);
	    setenv_hex("kernel_size",head.kernel_size);
	    setenv_hex("rootfs_size",head.rootfs_size);
		
        printf("flash_md5=%s\n", head.flash_md5);
        printf("kernel_md5=%s\n", head.kernel_md5);
        printf("rootfs_md5=%s\n", head.rootfs_md5);
        printf("kernel_size=%d\n", head.kernel_size);
        printf("rootfs_size=%d\n", head.rootfs_size);
        return 0;
    }
	return -1;
}


/*kernel rootfs md5 check*/
static int kernel_rootfs_md5_check()
{
    run_command("sf probe", 0);
    if(kernel_md5_check() != 0){
        return -1;
    }
    if(rootfs_md5_check() != 0){
        return -1;
    }
    return 0;
}

/*呼吸灯提示设备不能启动到系统，卡在了uboot*/
static void do_cvi_Breathinglight()
{
    int i = 1;
    int count = 40;
    for(;i < count;){
        cvi_update_led_control(0);
        mdelay(count-i);
        cvi_update_led_control(1);
        mdelay(i);
        i+=1;
    }
    mdelay(200);

    for(;i > 0;){
        cvi_update_led_control(1);
        mdelay(i);
        cvi_update_led_control(0);
        mdelay(count-i);
        i-=1;
    }
    mdelay(500);
}

typedef struct bootidx_info{
	char bootidx_str[64];	/*启动分区标记*/
	char netupdate_str[64];	/*刷机升级标记*/
	char serverip_str[64];	/*刷机tftp服务器地址*/
	char cleancfg_str[64];	/*刷机清空内容*/
}BOOTIDX_INFO;

typedef struct subarea_info{
	int keep_config;
	int protective_update;
}SUBAREA_INFO;



static int flash_write_img(int is_encrypt,int wait_reboot)
{
	int ret = -1;
	int update_flag = 0;
    int img_len = getenv_hex("filesize",0);
	char *buf = map_physmem((unsigned long)HEADER_ADDR, img_len, MAP_WRBACK);
	if (buf) {

		if(is_encrypt == 1){
			ret = ep_xxxtea_aes_decrypt_check(buf,img_len);
		}else{
			ret = 0;
		}
		if(ret == 0) {
			ret = -1;
			char *pcheck_buf = buf;
#ifdef CONFIG_NAND_SUPPORT
			pcheck_buf += 0x80000;
#endif
			printf("check head [%x,%x,%x,%x,%x,%x,%x,%x,%x]\n", pcheck_buf[0], pcheck_buf[1], pcheck_buf[2],
				   pcheck_buf[3],
				   pcheck_buf[4], pcheck_buf[5], pcheck_buf[6], pcheck_buf[7], pcheck_buf[8]);

			/*校验flash固件前9个字节是否包含CV1821*/
			if (((pcheck_buf[0] & 0x01) == 0x01) && ((pcheck_buf[1] & 0x00) == 0x00)
				&& ((pcheck_buf[2] & 0x64) == 0x64) && ((pcheck_buf[3] & 0xAA) == 0xAA)
				&& ((pcheck_buf[4] & 0x78) == 0x78) && ((pcheck_buf[5] & 0x56) == 0x56)
				&& ((pcheck_buf[6] & 0x34) == 0x34) && ((pcheck_buf[7] & 0x12) == 0x12)
				&& ((pcheck_buf[8] & 0x00) == 0x00)) {
				printf("check ok \n");
				ret = 0;
			}
		}
		unmap_physmem(buf, img_len);
	}
	if(ret == 0){
#ifdef CONFIG_NAND_SUPPORT

		char cmd[255] = { '\0' };
		ret = run_command("nand erase 0x0 ${filesize}", 0);
		if(ret == 0){
			printf("nand erase ok \n");
		}
		snprintf(cmd, 255, "nand write %p 0x0 ${filesize};",(void *) HEADER_ADDR);
		ret = run_command(cmd, 0);
		if(ret == 0){
			printf("nand write ok \n");
		}
		cvi_update_led_control_double(0);
		
#elif defined(CONFIG_SPI_FLASH)
		run_command("sf probe", 0);
		char cmd[255] = { '\0' };
		snprintf(cmd, 255,"sf update %p 0x0 ${filesize};",(void *) HEADER_ADDR);
		ret = run_command(cmd, 0);
#endif
	   
		update_flag = 1;
		cvi_update_led_control(1);
		
		while(wait_reboot){
			printf("The upgrade is complete and the device needs to be powered on again\n\n");
			mdelay(2000);
		}
	}
	return update_flag;
}

static int ep_get_update_flag(BOOTIDX_INFO *pinfo)
{

	char cmd[255] = { '\0' };
	int ret = -1;
#ifdef CONFIG_NAND_SUPPORT
	int update_offset = getenv_hex("UPDATE_PART_OFFSET",0);
	int update_offlen = getenv_hex("UPDATE_PART_SIZE",0);
	int map_len = sizeof(BOOTIDX_INFO);
	snprintf(cmd, 255, "nand read %p %#x %#x;",(void *) HEADER_ADDR,update_offset,update_offlen);
	printf("cmd = %s\n",cmd);
	ret = run_command(cmd, 0);
	if(ret == 0){
		char *buf = map_physmem((unsigned long)HEADER_ADDR, update_offlen, MAP_WRBACK);
		if (buf) {
			memcpy(pinfo,buf,map_len);
			unmap_physmem(buf, update_offlen);
		}
	}
	if(strstr(pinfo->bootidx_str,"bootidx")){
		printf("bootidx_str=%s\n",pinfo->bootidx_str);
		printf("netupdate_str=%s\n",pinfo->netupdate_str);
		printf("serverip_str=%s\n",pinfo->serverip_str);
		printf("cleancfg_str=%s\n",pinfo->cleancfg_str);
	}
	
#endif
	return ret;
}
static int ep_set_update_flag(BOOTIDX_INFO *pinfo)
{
	int ret = -1;
	char cmd[255] = { '\0' };
#ifdef CONFIG_NAND_SUPPORT
	
	int update_offset = getenv_hex("UPDATE_PART_OFFSET",0);
	int update_offlen = getenv_hex("UPDATE_PART_SIZE",0);
	int map_len = sizeof(BOOTIDX_INFO);
	snprintf(cmd, 255, "nand read %p %#x %#x;",(void *) HEADER_ADDR,update_offset,update_offlen);
	printf("cmd = %s\n",cmd);
	ret = run_command(cmd, 0);
	char *buf = map_physmem((unsigned long)HEADER_ADDR, update_offlen, MAP_WRBACK);
	if (buf) {
		memset(buf,0,map_len);
		memcpy(buf,pinfo,map_len);
		unmap_physmem(buf, update_offlen);
		char cmd1[64] = { '\0' };
		snprintf(cmd1, 64, "nand erase %#x %#x",update_offset,update_offlen);
		ret = run_command(cmd1, 0);
		if(ret == 0){
			printf("nand erase update flag ok \n");
		}
		char cmd2[255] = { '\0' };
		snprintf(cmd2, 255, "nand write %p %#x %#x;",(void *) HEADER_ADDR,update_offset,update_offlen);
		ret = run_command(cmd2, 0);
		if(ret == 0){
			printf("nand write update flag ok \n");
		}
	}

#endif	
	return ret;
}




static int ep_nand_flash_erase(int offset,int size)
{
	char cmd[255] = { '\0' };
	snprintf(cmd, 255, "nand erase %#x %#x;",offset,size);
	int ret = run_command(cmd, 0);
	if(ret != 0){
		printf("nand erase err \n");
		return ret;
	}
}


static int ep_nand_flash_write(int offset,int size)
{
	ep_nand_flash_erase(offset,size);
	char cmd[255] = { '\0' };
	snprintf(cmd, 255, "nand write %p %#x ${filesize};",(void *) HEADER_ADDR,offset);
	int ret = run_command(cmd, 0);
	if(ret != 0){
		printf("nand write err \n");
	}
	return ret;
}

/*升级kernel*/
static int dhcp_kernel_flash(char *ipaddr)
{
	char cmd2[255] = { '\0' };
	setenv_hex("filesize",0);
	snprintf(cmd2, 255, "dhcp %s:kernel.bin",ipaddr);
    int offset = getenv_hex("BOOT_PART_OFFSET",0);
	int size = getenv_hex("BOOT_PART_SIZE",0);
    int offset2 = getenv_hex("BOOT2_PART_OFFSET",0);
	int size2 = getenv_hex("BOOT2_PART_SIZE",0);
	int ret = run_command(cmd2, 0);
	if(ret == 0){
		int img_len = getenv_hex("filesize",0);
		if((img_len > 1024)&&(img_len < size)){
			ret = ep_nand_flash_write(offset,size);
			if(ret != 0){
				printf("ep_nand_flash_write kernel err,ret=%d \n",ret);
			}
			ret = ep_nand_flash_write(offset2,size2);
			if(ret != 0){
				printf("ep_nand_flash_write kernel2 err,ret=%d \n",ret);
			}
		}
	}
	return ret;
}

/*升级rootfs*/
static int dhcp_rootfs_flash(char *ipaddr)
{
	char cmd2[255] = { '\0' };
	setenv_hex("filesize",0);
	snprintf(cmd2, 255, "dhcp %s:rootfs.bin",ipaddr);
    int offset = getenv_hex("ROOTFS_PART_OFFSET",0);
	int size = getenv_hex("ROOTFS_PART_SIZE",0);
    int offset2 = getenv_hex("ROOTFS2_PART_OFFSET",0);
	int size2 = getenv_hex("ROOTFS2_PART_SIZE",0);
	int ret = run_command(cmd2, 0);
	if(ret == 0){
		int img_len = getenv_hex("filesize",0);
		if((img_len > 1024)&&(img_len < size)){
			ret = ep_nand_flash_write(offset,size);
			if(ret != 0){
				printf("ep_nand_flash_write rootfs err,ret=%d \n",ret);
			}
			ret = ep_nand_flash_write(offset2,size2);
			if(ret != 0){
				printf("ep_nand_flash_write rootfs2 err,ret=%d \n",ret);
			}
		}
	}
	return ret;
}

/*升级app*/
static int dhcp_app_flash(char *ipaddr)
{
	char cmd2[255] = { '\0' };
	setenv_hex("filesize",0);
	snprintf(cmd2, 255, "dhcp %s:app.bin",ipaddr);
    int offset = getenv_hex("APP_PART_OFFSET",0);
	int size = getenv_hex("APP_PART_SIZE",0);
    int offset2 = getenv_hex("APP2_PART_OFFSET",0);
	int size2 = getenv_hex("APP2_PART_SIZE",0);
	int ret = run_command(cmd2, 0);
	if(ret == 0){
		int img_len = getenv_hex("filesize",0);
		if((img_len > 1024)&&(img_len < size)){
			ret = ep_nand_flash_write(offset,size);
			if(ret != 0){
				printf("ep_nand_flash_write app err,ret=%d \n",ret);
			}
			ret = ep_nand_flash_write(offset2,size2);
			if(ret != 0){
				printf("ep_nand_flash_write app2 err,ret=%d \n",ret);
			}
		}
	}
	return ret;
}

static int ep_return_flag(char *ipaddr,char *name)
{
	char cmd2[255] = { '\0' };
	snprintf(cmd2, 255, "dhcp %s:%s",ipaddr,name);
	int ret = run_command(cmd2, 0);
	if(ret == 0){
		return 1;
	}
	return 0;
}


/*升级configs*/
static int dhcp_configs_flash(char *ipaddr,BOOTIDX_INFO  *pBootinfo,SUBAREA_INFO *pSucinfo)
{
	
	memset(pBootinfo->cleancfg_str,0,64);
	char cmd2[255] = { '\0' };
	setenv_hex("filesize",0);
	snprintf(cmd2, 255, "dhcp %s:flash_upgrade.cfg",ipaddr);
	int ret = run_command(cmd2, 0);
	if(ret == 0){
		int img_len = getenv_hex("filesize",0);
		char *buf = map_physmem((unsigned long)HEADER_ADDR, img_len, MAP_WRBACK);
		if(buf){
			if(strstr(buf,"protective=1")){
				pSucinfo->protective_update = 1;
			}
			if(strstr(buf,"retain=1")){
				pSucinfo->keep_config = 1;
			}
			char *tmp_str = strstr(buf,"clean=");
			if(tmp_str){
				memcpy(pBootinfo->cleancfg_str,tmp_str,64);
			}
			unmap_physmem(buf, img_len);
		}
	}
	return ret;
}


static int ep_nand_flash_write_subarea(int is_encrypt,SUBAREA_INFO *info)
{
	int ret = -1;
	int update_flag = 0;
    int img_len = getenv_hex("filesize",0);
	char *buf = map_physmem((unsigned long)HEADER_ADDR, img_len, MAP_WRBACK);
	if (buf) {
		if(is_encrypt == 1){
			ret = ep_xxxtea_aes_decrypt_check(buf,img_len);
		}else{
			ret = 0;
		}
		if((ret == 0)||(ret == -2)) {
			ret = -1;
			char *pcheck_buf = buf;
			pcheck_buf += 0x80000;
			printf("check head [%x,%x,%x,%x,%x,%x,%x,%x,%x]\n", pcheck_buf[0], pcheck_buf[1], pcheck_buf[2],
				   pcheck_buf[3],
				   pcheck_buf[4], pcheck_buf[5], pcheck_buf[6], pcheck_buf[7], pcheck_buf[8]);

			/*校验flash固件前9个字节是否包含CV1821*/
			if (((pcheck_buf[0] & 0x01) == 0x01) && ((pcheck_buf[1] & 0x00) == 0x00)
				&& ((pcheck_buf[2] & 0x64) == 0x64) && ((pcheck_buf[3] & 0xAA) == 0xAA)
				&& ((pcheck_buf[4] & 0x78) == 0x78) && ((pcheck_buf[5] & 0x56) == 0x56)
				&& ((pcheck_buf[6] & 0x34) == 0x34) && ((pcheck_buf[7] & 0x12) == 0x12)
				&& ((pcheck_buf[8] & 0x00) == 0x00)) {
				printf("check ok \n");
				ret = 0;
			}
		}
		unmap_physmem(buf, img_len);
	}
	if(ret == 0){
		int flash_len = getenv_hex("filesize",0);
		int up_offset = 0;
		int up_size = flash_len;
		if(info->protective_update){
			up_offset = getenv_hex("fip_PART_SIZE",0);
			up_size = flash_len - up_offset;
		}
		if(info->keep_config){
			int tmp_offset = getenv_hex("CONFIGS_PART_OFFSET",0);
			up_size = tmp_offset - up_offset;
		}
		
		char cmd[255] = { '\0' };
		snprintf(cmd, 255, "nand erase %#x %#x;",up_offset,up_size);
		ret = run_command(cmd, 0);
		if(ret == 0){
			printf("nand erase ok \n");
		}
		snprintf(cmd, 255, "nand write %p %#x %#x;",(void *) HEADER_ADDR+up_offset,up_offset,up_size);
		ret = run_command(cmd, 0);
		if(ret == 0){
			printf("nand write ok \n");
		}
		cvi_update_led_control_double(0);
		
		update_flag = 1;
		cvi_update_led_control(1);
	}
	return update_flag;
}


/*升级flash*/
static int dhcp_flash_flash(char *ipaddr,SUBAREA_INFO *pSucinfo)
{
	
	char cmd2[255] = { '\0' };
	setenv_hex("filesize",0);
	snprintf(cmd2, 255, "dhcp %s:ep_flash_sd_encrypt.bin",ipaddr);
	int ret = run_command(cmd2, 0);
	if(ret == 0){
		int img_len = getenv_hex("filesize",0);
		if(img_len > 1024){
			ret = ep_nand_flash_write_subarea(1,pSucinfo);
		}
	}
	return ret;
}




static int dhcp_update_flash(int key_status)
{
	char cmd[255] = { '\0' };
	int ret = 0;
	BOOTIDX_INFO  bootidxinfo = {0};
#ifdef CONFIG_NAND_SUPPORT
	run_command("env default -a", 0);
	ret = ep_get_update_flag(&bootidxinfo);
	if(ret == 0){			
		/*检测启动分区*/
		if(strstr(bootidxinfo.bootidx_str, "bootidx=1")){
			char *pnandboot = "setenv bootargs systemSelect=${systemSelect} mem=${mem} ubi.mtd=rootfs.B ubi.block=0,0 root=/dev/ubiblock0_0 rootfstype=squashfs ${mtdparts} console=$consoledev,$baudrate $othbootargs;nand read ${uimage_addr} ${BOOT2_PART_OFFSET}  ${BOOT2_PART_SIZE};mw.l 4330058 1 1; md.l 4330058 1; mw.l 3000154 0 1;bootm ${uimage_addr}#config@cv1821_wevb_0005b_spinand;";
			setenv("nandboot",pnandboot);
		}else{
			char *pnandboot = "setenv bootargs systemSelect=${systemSelect} mem=${mem} ubi.mtd=rootfs.A ubi.block=0,0 root=/dev/ubiblock0_0 rootfstype=squashfs ${mtdparts} console=$consoledev,$baudrate $othbootargs;nand read ${uimage_addr} ${BOOT_PART_OFFSET}  ${BOOT_PART_SIZE};mw.l 4330058 1 1; md.l 4330058 1; mw.l 3000154 0 1;bootm ${uimage_addr}#config@cv1821_wevb_0005b_spinand;";
			setenv("nandboot",pnandboot);
		}
		/*检测是否dhcp升级*/
		if(strstr(bootidxinfo.netupdate_str, "netupdate_flag=1") || key_status){
#ifdef CONFIG_NAND_SUPPORT					
			cvi_update_led_control_double(1);
#endif	
			char cmd2[255] = { '\0' };
			setenv_hex("filesize",0);
			if(key_status){
				snprintf(cmd2, 255, "%s","192.168.12.6");
			}else{
				snprintf(cmd2, 255, "%s",&bootidxinfo.serverip_str[9]);//serverip=192.168.28.4
			}
			SUBAREA_INFO suc_info = {0};
			dhcp_configs_flash(cmd2,&bootidxinfo,&suc_info);
			dhcp_flash_flash(cmd2,&suc_info);
			//dhcp_kernel_flash(cmd2);
			//dhcp_rootfs_flash(cmd2);
			//dhcp_app_flash(cmd2);
			//dhcp_configs_flash(cmd2,&bootidxinfo);

			strcpy(bootidxinfo.bootidx_str,"bootidx=0");
			strcpy(bootidxinfo.netupdate_str,"netupdate_flag=0");
			memset(bootidxinfo.serverip_str,0,64);
			ep_set_update_flag(&bootidxinfo);
			cvi_update_led_control(1);
			while(1){
				printf("The upgrade 2 is complete and the device needs to be powered on again\n\n");
				mdelay(2000);
			}
			//run_command("reset", 0);
		}
		
	}
	
#elif defined(CONFIG_SPI_FLASH)
	ret = -1;
#endif
	return ret;
}


static int sdcard_configs_flash(BOOTIDX_INFO  *pBootinfo,SUBAREA_INFO *pSucinfo)
{
	char cmd2[255] = { '\0' };
	setenv_hex("filesize",0);
	snprintf(cmd2, 255, "fatload mmc 1:1 %p flash_upgrade.cfg",(void *)HEADER_ADDR);
	int ret = run_command(cmd2, 0);
	if(ret == 0){
		int img_len = getenv_hex("filesize",0);
		char *buf = map_physmem((unsigned long)HEADER_ADDR, img_len, MAP_WRBACK);
		if(buf){
			if(strstr(buf,"protective=1")){
				pSucinfo->protective_update = 1;
			}
			if(strstr(buf,"retain=1")){
				pSucinfo->keep_config = 1;
			}
			char *tmp_str = strstr(buf,"clean=");
			if(tmp_str){
				memcpy(pBootinfo->cleancfg_str,tmp_str,64);
				
			}
			unmap_physmem(buf, img_len);
		}
	}
}

/*epflash 升级控制接口，
*参数 is_encrypt 是否支持加密验证升级
*参数 start_check 启动校验支持
*/
int ep_check_cvi_update(int is_encrypt,int start_check)
{
	SUBAREA_INFO suc_info = {0};
	BOOTIDX_INFO  bootidxinfo = {0};
	int update_flag = 0;
	int key_update = 0;

	int res = ep_get_update_flag(&bootidxinfo);
	if(res == 0){			
		/*检测启动分区*/
		if(strstr(bootidxinfo.bootidx_str, "bootidx=1")){
			char *pnandboot = "setenv bootargs systemSelect=${systemSelect} mem=${mem} ubi.mtd=rootfs.B ubi.block=0,0 root=/dev/ubiblock0_0 rootfstype=squashfs ${mtdparts} console=$consoledev,$baudrate $othbootargs;nand read ${uimage_addr} ${BOOT2_PART_OFFSET}  ${BOOT2_PART_SIZE};mw.l 4330058 1 1; md.l 4330058 1; mw.l 3000154 0 1;bootm ${uimage_addr}#config@cv1821_wevb_0005b_spinand;";
			setenv("nandboot",pnandboot);
		}else{
			char *pnandboot = "setenv bootargs systemSelect=${systemSelect} mem=${mem} ubi.mtd=rootfs.A ubi.block=0,0 root=/dev/ubiblock0_0 rootfstype=squashfs ${mtdparts} console=$consoledev,$baudrate $othbootargs;nand read ${uimage_addr} ${BOOT_PART_OFFSET}  ${BOOT_PART_SIZE};mw.l 4330058 1 1; md.l 4330058 1; mw.l 3000154 0 1;bootm ${uimage_addr}#config@cv1821_wevb_0005b_spinand;";
			setenv("nandboot",pnandboot);
		}
	}
	/*检测按钮是否按下，按下就认为是强制升级*/
	if(cvi_get_key_status() == 0){
		mdelay(300);
		if(cvi_get_key_status() == 0){
			sdcard_configs_flash(&bootidxinfo,&suc_info);
			key_update = 1;
		}
	}
	if(key_update == 1){
	    int ret = 1;
	    char cmd[255] = { '\0' };
		if(is_encrypt == 1){
	   		snprintf(cmd, 255, "fatload mmc 1:1 %p ep_flash_sd_encrypt.bin",(void *)HEADER_ADDR);
		}else{
			snprintf(cmd, 255, "fatload mmc 1:1 %p ep_flash_sd.bin",(void *)HEADER_ADDR);
		}
		ret = run_command(cmd, 0);
	    if(ret == 0) {				
			cvi_update_led_control_double(1);
			printf("keep_config=%d \n",suc_info.keep_config);
			printf("protective_update=%d \n",suc_info.protective_update);
			ret = ep_nand_flash_write_subarea(is_encrypt,&suc_info);
			strcpy(bootidxinfo.bootidx_str,"bootidx=0");
			strcpy(bootidxinfo.netupdate_str,"netupdate_flag=0");
			memset(bootidxinfo.serverip_str,0,64);
			ep_set_update_flag(&bootidxinfo);
			cvi_update_led_control(1);
			while(1){
				printf("The upgrade 2 is complete and the device needs to be powered on again\n\n");
				mdelay(2000);
			}
	    }
	}
    cvi_update_led_control_r(1);
    return 1;
}


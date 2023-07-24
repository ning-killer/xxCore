#define PINMUX_GPIO_A 	0x03020000
#define PINMUX_GPIO_B	0x03021000
#define PINMUX_GPIO_C 	0x03022000
#define PINMUX_GPIO_D 	0x03023000
#define PINMUX_GPIO_PWR 0x05021000

#define PINMUX_GPIO_BIT(n) (0x1<<n)

void  Gpio_Set_Value(uintptr_t base_addr,uint32_t bit, uint32_t value)
{
	uint32_t value_bak = readl(base_addr);
	uint32_t data_bit = value << bit;
	uint32_t data_ff = 1<<bit;
	data_ff = ~data_ff;
	value_bak &= data_ff;
	value_bak |= data_bit;
	//printf("value = %#x,bak=%#x \n",value,value_bak);
	writel(value_bak, base_addr);
}

int  Gpio_Get_Value(uintptr_t base_addr,uint32_t bit)
{
	uint32_t value_bak = readl(base_addr);
	uint32_t data_bit = value_bak >> bit;
	return data_bit;
}

void Gpio_Set_Dir(uintptr_t base_addr,uint32_t bit, uint32_t value)
{
	Gpio_Set_Value(base_addr|0x4,bit,value);
}

int cvi_update_led_control_r(int value)
{
	Gpio_Set_Value(PINMUX_GPIO_PWR, 22,value);
	return 0;
}

int cvi_update_led_control(int value)
{

	Gpio_Set_Value(PINMUX_GPIO_PWR, 22,value);
	return 0;
}

int cvi_update_led_control_double(int value)
{
	Gpio_Set_Value(PINMUX_GPIO_PWR, 22,value);
	Gpio_Set_Value(PINMUX_GPIO_PWR, 22,value);
	return 0;
}


int cvi_get_key_status()
{
	int ret = Gpio_Get_Value(PINMUX_GPIO_A|0x50,30);
	printf("get key status is %d\n",ret);
	return ret;
		
}

int cvi_board_init(void)
{
#if defined(CONFIG_MMC_SDHCI_CVITEK_WIFI)
	PINMUX_CONFIG(PWR_GPIO2, PWR_GPIO_2);
	PINMUX_CONFIG(PWR_WAKEUP0, PWR_GPIO_6);

        /* Pull WIFI_CHIP_EN to high */
        writel(readl(0x05021004) | WIFI_CHIP_EN_QFN, 0x05021004);
	writel(readl(0x05021000) | WIFI_CHIP_EN_QFN, 0x05021000);
#endif
	/*RCA_SW*/
	PINMUX_CONFIG(SD1_D1, PWR_GPIO_20);
	
	/*5pin reserved seat*/
	PINMUX_CONFIG(SD1_D0, PWR_GPIO_21);
	PINMUX_CONFIG(IIC0_SDA, XGPIOA_29);
	PINMUX_CONFIG(IIC0_SCL, XGPIOA_28);
	PINMUX_CONFIG(JTAG_CPU_TCK, XGPIOA_18);
	PINMUX_CONFIG(JTAG_CPU_TMS, XGPIOA_19);

	/*phy led*/
	//PINMUX_CONFIG(SD1_CMD, EPHY_LNK_LED);
	PINMUX_CONFIG(SD1_CMD, PWR_GPIO_22);
	Gpio_Set_Dir(PINMUX_GPIO_PWR,22,1);
	Gpio_Set_Value(PINMUX_GPIO_PWR, 22,1);

	/*IR_CUT*/
	PINMUX_CONFIG(SD1_D3, PWR_GPIO_18);
	PINMUX_CONFIG(SD1_CLK, PWR_GPIO_23);
	Gpio_Set_Dir(PINMUX_GPIO_PWR,18,1);
	Gpio_Set_Value(PINMUX_GPIO_PWR, 18,0);
	Gpio_Set_Dir(PINMUX_GPIO_PWR,23,1);
	Gpio_Set_Value(PINMUX_GPIO_PWR, 23,0);
	
	
	/*sensor clcok*/
	PINMUX_CONFIG(PAD_MIPI_TXP0, CAM_MCLK0);
	/*sensor opwer*/
	PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
	Gpio_Set_Dir(PINMUX_GPIO_C,12,1);
	Gpio_Set_Value(PINMUX_GPIO_C, 12,1);
	
	/*key*/
	PINMUX_CONFIG(AUX0,XGPIOA_30);

	/*usb wifi power*/
	Gpio_Set_Dir(PINMUX_GPIO_PWR,2,1);
	Gpio_Set_Value(PINMUX_GPIO_PWR, 2,1);

	/*w led*/
	Gpio_Set_Dir(PINMUX_GPIO_PWR,0,1);
	Gpio_Set_Value(PINMUX_GPIO_PWR, 0,1);

    return 0;
}

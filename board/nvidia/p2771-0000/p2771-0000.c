// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2016, NVIDIA CORPORATION
 */

#include <common.h>
#include <env.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <i2c.h>
#include <log.h>
#include <net.h>
#include <stdlib.h>
#include <linux/libfdt.h>
#include <asm/arch-tegra/board.h>
#include <asm/arch-tegra/smc.h>
#include "../p2571/max77620_init.h"

void pin_mux_mmc(void)
{
	struct udevice *dev;
	uchar val;
	int ret;

	/* Turn on MAX77620 LDO3 to 3.3V for SD card power */
	debug("%s: Set LDO3 for VDDIO_SDMMC_AP power to 3.3V\n", __func__);
	ret = i2c_get_chip_for_busnum(0, MAX77620_I2C_ADDR_7BIT, 1, &dev);
	if (ret) {
		printf("%s: Cannot find MAX77620 I2C chip\n", __func__);
		return;
	}
	/* 0xF2 for 3.3v, enabled: bit7:6 = 11 = enable, bit5:0 = voltage */
	val = 0xF2;
	ret = dm_i2c_write(dev, MAX77620_CNFG1_L3_REG, &val, 1);
	if (ret) {
		printf("i2c_write 0 0x3c 0x27 failed: %d\n", ret);
		return;
	}
}

#ifdef CONFIG_PCI_TEGRA
int tegra_pcie_board_init(void)
{
	struct udevice *dev;
	uchar val;
	int ret;

	/* Turn on MAX77620 LDO7 to 1.05V for PEX power */
	debug("%s: Set LDO7 for PEX power to 1.05V\n", __func__);
	ret = i2c_get_chip_for_busnum(0, MAX77620_I2C_ADDR_7BIT, 1, &dev);
	if (ret) {
		printf("%s: Cannot find MAX77620 I2C chip\n", __func__);
		return -1;
	}
	/* 0xC5 for 1.05v, enabled: bit7:6 = 11 = enable, bit5:0 = voltage */
	val = 0xC5;
	ret = dm_i2c_write(dev, MAX77620_CNFG1_L7_REG, &val, 1);
	if (ret)
		printf("i2c_write 0 0x3c 0x31 failed: %d\n", ret);

	return 0;
}
#endif

static const char * const nodes[] = {
	"/host1x@13e00000/display-hub@15200000/display@15200000",
	"/host1x@13e00000/display-hub@15200000/display@15210000",
	"/host1x@13e00000/display-hub@15200000/display@15220000",
};

static void ft_kaslr_setup(void *fdt)
{
	int offset, err;
	u8 seed[8];

	err = tegra_generate_random(seed, sizeof(seed));
	if (err < 0)
		return;

	err = fdt_check_header(fdt);
	if (err < 0)
		return;

	offset = fdt_find_or_add_subnode(fdt, 0, "chosen");
	if (offset < 0)
		return;

	err = fdt_setprop(fdt, offset, "kaslr-seed", seed, sizeof(seed));
	if (err < 0)
		printf("WARNING: can't set KASLR seed: %s\n", fdt_strerror(err));
}

int ft_board_setup(void *fdt, struct bd_info *bd)
{
	ft_mac_address_setup(fdt);
	ft_carveout_setup(fdt, nodes, ARRAY_SIZE(nodes));
	ft_kaslr_setup(fdt);

	return 0;
}

/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 */

#include <common.h>
#include <memalign.h>

#include <linux/arm-smccc.h>

#include <asm/arch-tegra/smc.h>

#define TEGRA_SMC_GENERATE_RANDOM 0x8200ff04

ssize_t tegra_smc_generate_random(void *buffer, size_t size)
{
	struct arm_smccc_res res;

	arm_smccc_smc(TEGRA_SMC_GENERATE_RANDOM, (unsigned long)buffer, size,
		      0, 0, 0, 0, 0, &res);

	return res.a0;
}

int tegra_generate_random(void *buffer, size_t size)
{
	size_t count = 0;
	ssize_t num;

	while (count < size) {
		num = tegra_smc_generate_random(buffer + count, size - count);
		if (num < 0)
			return num;

		count += num;
	}

	return 0;
}

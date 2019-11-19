/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 */

#ifndef TEGRA_SMC_H
#define TEGRA_SMC_H

ssize_t tegra_smc_generate_random(void *buffer, size_t size);
int tegra_generate_random(void *buffer, size_t size);

#endif /* TEGRA_SMC_H */

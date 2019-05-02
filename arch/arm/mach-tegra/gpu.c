// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2014-2015, NVIDIA CORPORATION.  All rights reserved.
 */

/* Tegra vpr routines */

#include <common.h>
#include <log.h>
#include <asm/io.h>
#include <asm/arch/tegra.h>
#include <asm/arch/mc.h>
#include <asm/arch-tegra/ap.h>

#include <fdt_support.h>

static bool tegra_vpr_locked(void)
{
	struct mc_ctlr *mc = (struct mc_ctlr *)NV_PA_MC_BASE;
	u32 value = readl(&mc->mc_video_protect_reg_ctrl);

	if (value & TEGRA_MC_VIDEO_PROTECT_REG_WRITE_ACCESS_DISABLED)
		return true;

	return false;
}

void tegra_gpu_config(void)
{
	struct mc_ctlr *mc = (struct mc_ctlr *)NV_PA_MC_BASE;

	if (tegra_vpr_locked())
		return;

#if defined(CONFIG_TEGRA_SUPPORT_NON_SECURE)
	if (!tegra_cpu_is_non_secure())
#endif
	{
		/* Turn VPR off */
		writel(0, &mc->mc_video_protect_size_mb);
		writel(TEGRA_MC_VIDEO_PROTECT_REG_WRITE_ACCESS_DISABLED,
		       &mc->mc_video_protect_reg_ctrl);
		/* read back to ensure the write went through */
		readl(&mc->mc_video_protect_reg_ctrl);
	}

	debug("configured VPR\n");
}

#if defined(CONFIG_OF_LIBFDT)

int tegra_gpu_enable_node(void *blob, const char *compat)
{
	int offset;

	if (!tegra_vpr_locked())
		return 0;

	offset = fdt_node_offset_by_compatible(blob, -1, compat);
	while (offset != -FDT_ERR_NOTFOUND) {
		fdt_status_okay(blob, offset);
		offset = fdt_node_offset_by_compatible(blob, offset, compat);
	}

	return 0;
}

#endif

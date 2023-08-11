#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/io.h>

#include "fih_ramtable.h"
#include "fih_hwcfg.h"

/* conserve current hwcfg table in kernel use */
struct st_hwcfg_info fih_hwcfg_table = {0};

void fih_hwcfg_print(struct st_hwcfg_info *tb)
{
	pr_info("fih_hwcfg_print: begin\n");

	if (NULL == tb) {
		pr_err("fih_hwcfg_print: tb = NULL\n");
		goto out;
	}

	pr_info("hwid = %d\n", tb->hwid);
	pr_info("project = %d\n", tb->project);
	pr_info("hw_rev = %d\n", tb->hw_rev);
	pr_info("rf = %d\n", tb->rf);
	pr_info("bsp = %d\n", tb->bsp);
	pr_info("dtm = %d\n", tb->dtm);
  pr_info("dtn = %d\n", tb->dtn);
  pr_info("sim = %d\n", tb->sim);
	/* info */
	pr_info("project_name = %s\n", tb->project_name);
	pr_info("factory_name = %s\n", tb->factory_name);
  pr_info("phase_sw = %s\n", tb->phase_sw);
  pr_info("phase_hw = %s\n", tb->phase_hw);
  pr_info("sku_name = %s\n", tb->sku_name);
  pr_info("rf_band = %s\n", tb->rf_band);

out:
	pr_info("fih_hwcfg_print: end\n");
}

void fih_hwcfg_setup(void)
{
  struct st_hwcfg_info *buf = (struct st_hwcfg_info *)ioremap(FIH_HWCFG_MEM_ADDR, FIH_HWCFG_MEM_SIZE);
	if (buf == NULL) {
		pr_err("%s: setup hwcfg table by default coded because load fail\n", __func__);
		return;
	}

	pr_info("%s: read hwcfg table from memory\n", __func__);
	memcpy(&fih_hwcfg_table, buf, sizeof(struct st_hwcfg_info));

	fih_hwcfg_print(&fih_hwcfg_table);
}

void* fih_hwcfg_fetch(int idx)
{
	struct st_hwcfg_info *tb = (struct st_hwcfg_info *)&fih_hwcfg_table;
  void* ret = NULL;

	switch (idx) {
		case FIH_HWCFG_INT_PROJECT: ret = (void *)&(tb->project); break;
    case FIH_HWCFG_INT_HWREV: ret = (void *)&(tb->hw_rev); break;
		case FIH_HWCFG_INT_BSP: ret = (void *)&(tb->rf); break;
		case FIH_HWCFG_INT_RF: ret = (void *)&(tb->bsp); break;
		case FIH_HWCFG_INT_SIM: ret = (void *)&(tb->sim); break;
		case FIH_HWCFG_INT_DTM: ret = (void *)&(tb->dtm); break;
		case FIH_HWCFG_INT_DTN: ret = (void *)&(tb->dtn); break;
    case FIH_HWCFG_INT_HWID: ret = (void *)&(tb->hwid); break;
		/* device tree */
		case FIH_HWCFG_CHAR_PROJECT: ret = (void *)tb->project_name; break;
		case FIH_HWCFG_CHAR_FACTORY: ret = (void *)tb->factory_name; break;
    case FIH_HWCFG_CHAR_PHASE_SW: ret = (void *)tb->phase_sw; break;
    case FIH_HWCFG_CHAR_PHASE_HW: ret = (void *)tb->phase_hw; break;
    case FIH_HWCFG_CHAR_SKU: ret = (void *)tb->sku_name; break;
    case FIH_HWCFG_CHAR_RF_BAND: ret = (void *)tb->rf_band; break;
    case FIH_HWCFG_CHAR_BSP: ret = (void *)tb->pcba_description; break;
		default:
			pr_warn("fih_hwcfg_fetch: unknown idx = %d\n", idx);
			ret = NULL;
			break;
	}

	return ret;
}

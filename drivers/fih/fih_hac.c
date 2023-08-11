#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <fih/hwid.h>
#include <linux/seq_file.h>

static int fih_info_proc_hac_show(struct seq_file *m, void *v)
{
	char msg[8];

	strcpy(msg, "HAC");
	seq_printf(m, "%s\n", msg);

	return 0;
}

static int fih_info_proc_hac(struct inode *inode, struct file *file)
{
	return single_open(file, fih_info_proc_hac_show, NULL);
}

/* This structure gather "function" that manage the /proc file
 */
static const struct file_operations hac_file_ops = {
	.owner   = THIS_MODULE,
	.open	 = fih_info_proc_hac,
	.read    = seq_read
};

static int __init fih_hac_init(void)
{
  if ((fih_hwid_fetch(FIH_HWID_PRJ) == FIH_PRJ_EAG) || (fih_hwid_fetch(FIH_HWID_PRJ) == FIH_PRJ_RHD))
  {
	  if (proc_create("Hac", 0, NULL, &hac_file_ops) == NULL) {
		  pr_err("fail to create proc/Hac\n");
	  }
  }
	return (0);
}

static void __exit fih_hac_exit(void)
{
  if ((fih_hwid_fetch(FIH_HWID_PRJ) == FIH_PRJ_EAG) || (fih_hwid_fetch(FIH_HWID_PRJ) == FIH_PRJ_RHD))
  {
	  remove_proc_entry("Hac", NULL);
  }
}

module_init(fih_hac_init);
module_exit(fih_hac_exit);

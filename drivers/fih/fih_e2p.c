#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/io.h>
#include <fih/share/mfd.h>
#include "fih_ramtable.h"

#define FIH_E2P_DATA_ST  FIH_MFD_V0

#define FIH_E2P_ST_ADDR  FIH_E2P_BASE
#define FIH_E2P_ST_SIZE  FIH_E2P_SIZE

static char pid[FIH_MFD_DATA_LEN] = "0";
static char bt_mac_addr[FIH_MFD_DATA_LEN] = "0";
static char wifi_mac_addr[FIH_MFD_DATA_LEN] = "0";
static char imei1[FIH_MFD_DATA_LEN] = "0";
static char imei2[FIH_MFD_DATA_LEN] = "0";
static char simconfig[FIH_MFD_DATA_LEN] = "0";
static char batteryinfo[FIH_MFD_DATA_LEN] = "0";
static char bt2_mac_addr[FIH_MFD_DATA_LEN] = "0";

int fih_e2p_setup(void)
{
	FIH_E2P_DATA_ST *e2p;

	e2p = (FIH_E2P_DATA_ST *)ioremap(FIH_E2P_ST_ADDR, sizeof(FIH_E2P_DATA_ST));
	if (e2p == NULL) {
		pr_err("%s: ioremap fail\n", __func__);
		return (0);
	}

	sprintf(pid, "%s", e2p->item[FIH_MFD_ITEM_PID].data);
  sprintf(bt_mac_addr, "%s", e2p->item[FIH_MFD_ITEM_BT_MAC].data);
  sprintf(wifi_mac_addr, "%s", e2p->item[FIH_MFD_ITEM_WIFI_MAC].data);
	sprintf(imei1, "%s", e2p->item[FIH_MFD_ITEM_IMEI_1].data);
	sprintf(imei2, "%s", e2p->item[FIH_MFD_ITEM_IMEI_2].data);
	sprintf(simconfig, "%s", e2p->item[FIH_MFD_ITEM_SIMCONFIG].data);
	sprintf(batteryinfo, "%s", e2p->item[FIH_MFD_ITEM_BATT_INFO].data);
	sprintf(bt2_mac_addr, "%s", e2p->item[FIH_MFD_ITEM_BT2_MAC].data);
	
	iounmap(e2p);

	return 0;

}

static int fih_e2p_proc_read_pid_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", pid);

	return 0;
}

static int productid_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_e2p_proc_read_pid_show, NULL);
};

static struct file_operations productid_file_ops = {
	.owner   = THIS_MODULE,
	.open    = productid_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int fih_e2p_proc_read_bt_mac_show(struct seq_file *m, void *v)
{
	char tmp[(FIH_MFD_DATA_LEN * 2)];
	unsigned int i, k;

	/* 0123456789AB -> 01:23:45:67:89:AB */
	memset(tmp, 0, sizeof(tmp));
	k = 0;
	for (i=0; i<strlen(bt_mac_addr); i++) {
		if ((i > 0)&&((i % 2) == 0)) tmp[k++] = ':';
		tmp[k++] = bt_mac_addr[i];
	}
  seq_printf(m, "%s\n", tmp);

	return 0;
}

static int bt_mac_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_e2p_proc_read_bt_mac_show, NULL);
};

static struct file_operations bt_mac_file_ops = {
	.owner   = THIS_MODULE,
	.open    = bt_mac_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int fih_e2p_proc_read_wifi_mac_show(struct seq_file *m, void *v)
{
	char tmp[(FIH_MFD_DATA_LEN * 2)];
	unsigned int i, k;

	/* 0123456789AB -> 01:23:45:67:89:AB */
	memset(tmp, 0, sizeof(tmp));
	k = 0;
	for (i=0; i<strlen(wifi_mac_addr); i++) {
		if ((i > 0)&&((i % 2) == 0)) tmp[k++] = ':';
		tmp[k++] = wifi_mac_addr[i];
	}
  seq_printf(m, "%s\n", tmp);

	return 0;
}

static int fih_e2p_proc_read_sim_mode_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n",simconfig);

	return 0;
}

static int wifi_mac_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_e2p_proc_read_wifi_mac_show, NULL);
};

static int sim_mode_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_e2p_proc_read_sim_mode_show, NULL);
};

static struct file_operations wifi_mac_file_ops = {
	.owner   = THIS_MODULE,
	.open    = wifi_mac_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static struct file_operations sim_mode_file_ops = {
	.owner   = THIS_MODULE,
	.open    = sim_mode_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int fih_proc_read_imei1_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", imei1);
	return 0;
}

static int imei_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_proc_read_imei1_show, NULL);
};

static struct file_operations imei_file_ops = {
	.owner   = THIS_MODULE,
	.open    = imei_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int fih_proc_read_imei2_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", imei2);
	return 0;
}

static int imei2_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_proc_read_imei2_show, NULL);
};

static struct file_operations imei2_file_ops = {
	.owner   = THIS_MODULE,
	.open    = imei2_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int fih_proc_read_batteryinfo_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", batteryinfo);
	return 0;
}

static int batteryinfo_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_proc_read_batteryinfo_show, NULL);
};

static struct file_operations batteryinfo_file_ops = {
	.owner   = THIS_MODULE,
	.open    = batteryinfo_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int fih_proc_read_bt2_mac_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", bt2_mac_addr);
	return 0;
}

static int bt2_mac_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_proc_read_bt2_mac_show, NULL);
};

static struct file_operations bt2_mac_file_ops = {
	.owner   = THIS_MODULE,
	.open    = bt2_mac_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static struct {
		char *name;
		struct file_operations *ops;
} *p, fih_e2p[] = {
	{"productid", &productid_file_ops},
	{"bt_mac", &bt_mac_file_ops},
	{"wifi_mac", &wifi_mac_file_ops},
	{"simmode", &sim_mode_file_ops},
	{"imei1", &imei_file_ops},
	{"imei2", &imei2_file_ops},
	{"batteryinfo", &batteryinfo_file_ops},
	{"bt2_mac", &bt2_mac_file_ops},
	{NULL}, };

static int __init fih_e2p_init(void)
{
	if (FIH_E2P_ST_SIZE < sizeof(FIH_E2P_DATA_ST)) {
		pr_err("%s: WARNNING!! FIH_E2P_ST_SIZE (%d) < sizeof(FIH_E2P_DATA_ST) (%lu)",
			__func__, FIH_E2P_ST_SIZE, sizeof(FIH_E2P_DATA_ST));
		return (1);
	}

  fih_e2p_setup();

	for (p = fih_e2p; p->name; p++) {
		if (proc_create(p->name, 0, NULL, p->ops) == NULL) {
			pr_err("fail to create proc/%s\n", p->name);
		}
	}

	return (0);
}

static void __exit fih_e2p_exit(void)
{
	for (p = fih_e2p; p->name; p++) {
		remove_proc_entry(p->name, NULL);
	}
}

module_init(fih_e2p_init);
module_exit(fih_e2p_exit);

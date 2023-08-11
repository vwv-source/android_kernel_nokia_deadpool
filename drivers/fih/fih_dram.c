#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/io.h>
#include <linux/mm.h>
#include "fih_ramtable.h"

#define FIH_PROC_DIR   "AllHWList"
#define FIH_PROC_PATH  "AllHWList/draminfo"
#define FIH_PROC_TESTRESULT_PATH  "AllHWList/dramtest_result"//HCLai add for memory test in RUNIN
#define FIH_PROC_SIZE  32

static char fih_proc_data[FIH_PROC_SIZE] = "";
static char fih_proc_test_result[FIH_PROC_SIZE] = {0};//HCLai add for memory test in RUNIN

/*-------------------------------------------------------------*/
#define FIH_MEM_ST_HEAD  0x48464d45  /* HFME */
#define FIH_MEM_ST_TAIL  0x454d4654  /* EMFT */

struct st_fih_mem {
	unsigned int head;
	unsigned int mfr_id;
	unsigned int ddr_type;
	unsigned int size_mb;
	unsigned int test_result;//HCLai add for memory test in RUNIN
	unsigned int tail;
};
struct st_fih_mem dram;

static void fih_dram_setup_MEM(void)
{
	char *buf = fih_proc_data;
	struct st_fih_mem *p;
	char size[16];
	long DRAM_Test_result=0;
	/* get dram in mem which lk write */
	p = (struct st_fih_mem *)ioremap(FIH_MEM_MEM_ADDR, sizeof(struct st_fih_mem));
	if (p == NULL) {
		pr_err("%s: ioremap fail\n", __func__);
		dram.head = FIH_MEM_ST_HEAD;
		dram.mfr_id = 0;
		dram.ddr_type = 0;
		dram.size_mb = 0;
		dram.test_result = 0;//HCLai add for memory test in RUNIN
		dram.tail = FIH_MEM_ST_TAIL;
	} else {
		memcpy(&dram, p, sizeof(struct st_fih_mem));
		iounmap(p);
	}

	/* check magic of dram */
	if ((dram.head != FIH_MEM_ST_HEAD)||(dram.tail != FIH_MEM_ST_TAIL)) {
		pr_err("%s: bad magic\n", __func__);
		dram.head = FIH_MEM_ST_HEAD;
		dram.mfr_id = 0;
		dram.ddr_type = 0;
		dram.size_mb = 0;
		dram.test_result = 0;//HCLai add for memory test in RUNIN
		dram.tail = FIH_MEM_ST_TAIL;
	}

	switch (dram.mfr_id) {
		case 0x01: strcat(buf, "SAMSUNG"); break;
		case 0x03: strcat(buf, "MICRON"); break;
		case 0x06: strcat(buf, "SK-HYNIX"); break;
		case 0xFF: strcat(buf, "MICRON"); break;
		default: strcat(buf, "UNKNOWN"); break;
	}

	switch (dram.ddr_type) {
		case 2: strcat(buf, " LPDDR2"); break;
		case 5: strcat(buf, " LPDDR3"); break;
		default: strcat(buf, " LPDDRX"); break;
	}

	snprintf(size, sizeof(size), " %dMB", dram.size_mb);
	strcat(buf, size);

	//HCLai add for memory test in RUNIN START
	//To-Do: use switch case to identify PASS/FAIL
	DRAM_Test_result = dram.test_result;
	printk("DRAM_Test_result is %ld\n",DRAM_Test_result);
	if(DRAM_Test_result==0)
		snprintf(fih_proc_test_result, sizeof(fih_proc_test_result), "%d", dram.test_result);
	else
		snprintf(fih_proc_test_result, sizeof(fih_proc_test_result), "0x%x", dram.test_result);
	//HCLai add for memory test in RUNIN END

}


//HCLai add for memory test in RUNIN START
/*-------------------------------------------------------------*/
static int fih_proc_test_result_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", fih_proc_test_result);
	return 0;
}

static int draminfo_test_result_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_proc_test_result_show, NULL);
};

static struct file_operations draminfo_test_result_ops = {
	.owner   = THIS_MODULE,
	.open    = draminfo_test_result_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};
//HCLai add for memory test in RUNIN END

/*-------------------------------------------------------------*/

static int fih_proc_read_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", fih_proc_data);
	return 0;
}

static int draminfo_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_proc_read_show, NULL);
};

static struct file_operations draminfo_file_ops = {
	.owner   = THIS_MODULE,
	.open    = draminfo_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int __init fih_proc_init(void)
{
	//fih_dram_setup();
	fih_dram_setup_MEM();

	if (proc_create(FIH_PROC_PATH, 0, NULL, &draminfo_file_ops) == NULL) {
		pr_err("fail to create proc/%s\n", FIH_PROC_PATH);
		return (1);
	}
	//HCLai add for memory test in RUNIN START
	if (proc_create(FIH_PROC_TESTRESULT_PATH, 0, NULL, &draminfo_test_result_ops) == NULL) {
		pr_err("fail to create proc/%s\n", FIH_PROC_PATH);
		return (1);
	}
	//HCLai add for memory test in RUNIN END
	return (0);
}

static void __exit fih_proc_exit(void)
{
	remove_proc_entry(FIH_PROC_PATH, NULL);
}

module_init(fih_proc_init);
module_exit(fih_proc_exit);

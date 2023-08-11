#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/io.h>
#include "fih_ramtable.h"
#include <linux/uaccess.h>


static char *my_proc_name = "rsutoken";
static unsigned int my_proc_addr = FIH_RSUTOKEN_BASE;
static unsigned int my_proc_size = 152;
static unsigned int my_proc_len = 152;
static unsigned int my_proc_offset = 0x400; //1KB

/* This function is called at the beginning of a sequence.
 * ie, when:
 * - the /proc file is read (first time)
 * - after the function stop (end of sequence)
 */
static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
	if (((*pos)*PAGE_SIZE) >= my_proc_len) return NULL;
	return (void *)((unsigned long) *pos+1);
}

/* This function is called after the beginning of a sequence.
 * It's called untill the return is NULL (this ends the sequence).
 */
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	++*pos;
	return my_seq_start(s, pos);
}

/* This function is called at the end of a sequence
 */
static void my_seq_stop(struct seq_file *s, void *v)
{
	/* nothing to do, we use a static value in start() */
}

/* This function is called for each "step" of a sequence
 */
static int my_seq_show(struct seq_file *s, void *v)
{
	//long n = (long)v-1;
	char *buf = (char *)ioremap(my_proc_addr, my_proc_size);
#ifdef RSUTOKEN_DEBUG
	int i=0;
#endif
	if (buf == NULL) {
		return 0;
	}
#ifdef RSUTOKEN_DEBUG
	printk("%s debug 2 my_seq_show----\n", __func__);
	for (i=0;i<my_proc_size;i++) {
		printk("buf[%d]=%d\n", i, (char )buf[i]);
	}
}
#endif
	seq_write(s, buf, my_proc_size);
	iounmap(buf);

	return 0;
}

/* This structure gather "function" to manage the sequence
 */
static struct seq_operations my_seq_ops = {
	.start = my_seq_start,
	.next  = my_seq_next,
	.stop  = my_seq_stop,
	.show  = my_seq_show
};

/* This function is called when the /proc file is open.
 */
static int my_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &my_seq_ops);
};

static ssize_t _seq_write(struct file *file, const char  __user *buffer,
					size_t count, loff_t *data)
{
	char tmp[152] = {0};
	int len = my_proc_len;
	char *buf = NULL;
	int i=0;

	if (count < len) len = count;
	if ( copy_from_user(tmp, buffer, count) ) {
		return -EFAULT;
	}
	buf = (char *)ioremap(my_proc_addr, my_proc_size);
	if (buf == NULL) {
		return 0;
	}
#ifdef RSUTOKEN_DEBUG
	printk("%s debug 1 _seq_write----\n", __func__);
#endif
	for (i=0;i<my_proc_size;i++) {
#ifdef RSUTOKEN_DEBUG
		printk("tmp[%d]=%d\n", i, (char )tmp[i]);
#endif
		buf[i]=tmp[i];
	}
	iounmap(buf);

	return count;
}


/* This structure gather "function" that manage the /proc file
 */
static struct file_operations my_file_ops = {
	.owner   = THIS_MODULE,
	.open    = my_open,
	.read    = seq_read,
	.write    = _seq_write,
	.llseek  = seq_lseek,
	.release = seq_release
};

/* This function is called when the module is loaded
 */
static int __init my_module_init(void)
{
	struct proc_dir_entry *entry;

	my_proc_addr = FIH_RSUTOKEN_BASE + my_proc_offset;
	my_proc_len = my_proc_size;

	entry = proc_create(my_proc_name, 0644, NULL, &my_file_ops);
	if (!entry) {
		pr_err("%s: fail create %s proc\n", my_proc_name, my_proc_name);
	}

	return 0;
}

/* This function is called when the module is unloaded.
 */
static void __exit my_module_exit(void)
{
	remove_proc_entry(my_proc_name, NULL);
}

module_init(my_module_init);
module_exit(my_module_exit);

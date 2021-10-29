#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/device.h>
#include <mach/gpio.h>
#include <linux/interrupt.h>
#include <linux/poll.h>


static struct class *botton_class;
static struct class_device *botton_device_class;

volatile unsigned long *gpfcon;
volatile unsigned long *gpfdat;

volatile unsigned long *gpgcon;
volatile unsigned long *gpgdat;




static int bottons_open (struct inode *inode, struct file *file)
{
	/* 配置GPF0,2为输入引脚 */
	*gpfcon &= ~((0x3<<(0*2)) | (0x3<<(2*2)));

	/* 配置GPG3,11为输入引脚 */
	*gpgcon &= ~((0x3<<(3*2)) | (0x3<<(11*2)));

	return 0;
}

static ssize_t bottons_read (struct file *file, char __user *buf, size_t size, loff_t  *ppos)
{
	/* 返回4个引脚的电平 */
	unsigned char key_vals[4];
	int regval;

	if (size != sizeof(key_vals))
		return -EINVAL;

	/* 读GPF0,2 */
	regval = *gpfdat;
	key_vals[0] = (regval & (1<<0)) ? 1 : 0;
	key_vals[1] = (regval & (1<<2)) ? 1 : 0;
	

	/* 读GPG3,11 */
	regval = *gpgdat;
	key_vals[2] = (regval & (1<<3)) ? 1 : 0;
	key_vals[3] = (regval & (1<<11)) ? 1 : 0;

	copy_to_user(buf, key_vals, sizeof(key_vals));
	
	return sizeof(key_vals);
}


/*define a file_operation struct */

static struct file_operations bottons_drv = {
	.owner    = THIS_MODULE,
	.open     = bottons_open,
	.read     = bottons_read,
};


/*set init and exit function*/
int major;
static int __init bottons_init(void)
{
	major =  register_chrdev(0, "bottons", &bottons_drv);

	botton_class        = class_create(THIS_MODULE, "bottons");
	botton_device_class = device_create(botton_class, NULL, MKDEV(major, 0), NULL, "bottons");
	
	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;

	gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
	gpgdat = gpgcon + 1;	
	return 0;
}

static void __exit bottons_exit(void)
{
	unregister_chrdev(major, "bottons");
	device_destroy(botton_class,  MKDEV(major, 0));
	class_destroy(botton_class);
	iounmap(gpfcon);
	iounmap(gpgcon);

}
module_init(bottons_init);
module_exit(bottons_exit);
MODULE_LICENSE("GPL");




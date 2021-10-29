


static struct file_operations s3c24xx_leds_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   s3c24xx_leds_open,     
	.read	=	s3c24xx_leds_read,	   
	.write	=	s3c24xx_leds_write,	   
};




static void __init myled_init(void)
{
	driver_register(&s3c24xx_leds_fops);
}

static void __exit myled_exit(void)
{
	driver_unregister(&s3c24xx_leds_fops);
}




module_init(myled_init);
module_exit(myled_exit);

MODULE_LICENSE("GPL v2");




#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <asm-generic/uaccess.h>


#define HC_SR04_CNT 1
#define HC_SR04_NAME "hc_sr04_drvs"


/*hc_sr04 设备结构体*/
static struct hc_sr04 {
	dev_t devid; //设备号
	struct cdev cdev; //字符设备核心结构体
	struct class *class;
	struct device *device;
	struct device_node *nd; /*设备节点*/
	int irq_echo;
	int gpio_echo;
	int gpio_trig;
	struct tasklet_struct tasklet;
	struct work_struct work;
	wait_queue_head_t wq;
	int data_ready;
	u32 start_us;
	u32 end_us;
	u32 distance_cm;
	
} hc_sr04;


static int hc_sr04_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &hc_sr04;



	return 0;
}


static ssize_t hc_sr04_write (struct file * filp, const char __user * buf, size_t count, loff_t *ppos)
{	
	struct hc_sr04 *dev = (struct  hc_sr04 *)filp->private_data;

	return 0;
}

static int hc_sr04_release(struct inode *inode, struct file *filp) {

	struct led *dev = (struct led *)filp->private_data;

	return 0;
}

static ssize_t hc_sr04_read (struct file *filp, char __user *buf , size_t count , loff_t * arg)
{

	
	int ret = 0;
	struct hc_sr04 *dev = filp->private_data;
	dev->data_ready = 0;
	
	unsigned long flags;
   
   

	 /* 启动触发信号 */
    gpio_set_value(hc_sr04.gpio_trig, 1);  
    udelay(15);
    gpio_set_value(hc_sr04.gpio_trig, 0);

	wait_event_interruptible_timeout(dev->wq, dev->data_ready, msecs_to_jiffies(60));

	
	if (count > 4) count = 4;
	
	printk("kernel:%d\n",dev->distance_cm);
	if (dev->data_ready == 1) {
		if (copy_to_user(buf, &dev->distance_cm, count) != 0) {
			return -EFAULT;

		} else {
			ret = count;
		}
	} else {
		ret  = -ETIMEDOUT;
	}
		

	
	
	return ret;
}

/* 
	设备文件操作集，应用层open/dev/xxx后，使用wrtie、open、close等系统函数时就会调用对应下面的函数，
	所以应用层调用read、write等系统调用的返回值什么的都是由下列函数自己确定的，
	而不是像打开一个普通的文件，read默认是阻塞的，你想返回什么你可以自己规定，
	应用层想正常使用就得按照你的要求来，逼格真高。
*/
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = hc_sr04_write,//write
	.open = hc_sr04_open,//open
	.release =  hc_sr04_release,//close
	.read  = hc_sr04_read,
};

static irqreturn_t hc_sr04_irq_handler(int irq, void *dev_id)
{
    struct hc_sr04 *dev = (struct hc_sr04 *)dev_id;
    unsigned long flags;

  
	if (gpio_get_value(dev->gpio_echo) == 1) {
		dev->start_us = ktime_to_us(ktime_get());
	} else {
		dev->end_us = ktime_to_us(ktime_get());	
		if (!work_pending(&dev->work))
			schedule_work(&dev->work);
	}
   	

  
	
    return IRQ_HANDLED;
}

static void hc_sr04_work_handler(struct work_struct *work)
{
	
	
	
	
	struct hc_sr04 *dev = container_of(work, struct hc_sr04, work);
	u32 time_us = dev->end_us - dev->start_us;
	
	dev->distance_cm = time_us * 34 / 2000;;
	if (dev->distance_cm > 6 && dev->distance_cm < 400) {
		dev->data_ready = 1;
	}
	
	

	wake_up_interruptible(&dev->wq);

	
	
	
}

static int hc_sr04_probe(struct platform_device *dev) {
	
	
		
		printk("hc_sr04_probe\n");
	
		/*	 1， 申请设备号		 */
		
		int ret = 0;
	
			//动态申请设备号
		ret = alloc_chrdev_region(&hc_sr04.devid, 0, HC_SR04_CNT, HC_SR04_NAME);
		if (ret < 0) {
			printk("设备号申请失败\n");
			goto faile_devid;
		}
	
		/*	2，添加字符设备	  */
		
		hc_sr04.cdev.owner = THIS_MODULE;
		
			//关键的一行，把文件操作集和设备文件绑定
		cdev_init(&hc_sr04.cdev, &fops);
		ret = cdev_add(&hc_sr04.cdev, hc_sr04.devid, HC_SR04_CNT);
		if (ret < 0) {
			goto faile_cdev;
		}
	
		/*3, 创建设备节点*/
		
			//创建/sys/class/xxx
		hc_sr04.class = class_create(THIS_MODULE, HC_SR04_NAME);
		if (IS_ERR(hc_sr04.class)) {
			ret = PTR_ERR(hc_sr04.class);
	
			goto faile_class;
		}
			//创建/dev/xxx
		hc_sr04.device = device_create(hc_sr04.class, NULL, hc_sr04.devid, NULL, HC_SR04_NAME);
		if (IS_ERR(hc_sr04.device)) {
			ret = PTR_ERR(hc_sr04.class);
			goto faile_device;
		}
		
		/*
			关键部分，一定别忘了return 0，否则会执行下面的释放代码，然后会在卸载驱动的时候会再次释放，
			此时内核卡死，必须重启,我以前就踩过这个坑，好几百行代码里找不到错误
		  
		*/
		
		
		/*获取设备树的属性内容*/
			//现代驱动一般都是跟设备树结合，这里省略，后面会复习到
	
		hc_sr04.nd = of_find_node_by_path("/hc_sr04");
		if (hc_sr04.nd == NULL) {
			printk("device node：hc_sr04 not found\n");
			goto faile_device_nd;	
		}

		
		hc_sr04.gpio_echo = of_get_named_gpio(hc_sr04.nd, "gpio-echo", 0);
		if (hc_sr04.gpio_echo < 0) {
			printk("get hc_sr04 gpio_echo error\n");
			ret = -EINVAL;
			goto faile_device; 
		}
		printk("hc-sr04:gpio-echo:%d\n", hc_sr04.gpio_echo);

		ret = gpio_request(hc_sr04.gpio_echo, "hc-sr04 echo");
		
		if (ret != 0) {
				printk("hc-sr04:gpio-echo request error\n");
				goto faile_device;
		}
		hc_sr04.gpio_trig = of_get_named_gpio(hc_sr04.nd, "gpio-trig", 0);
		if (hc_sr04.gpio_trig < 0) {
			printk("get hc_sr04 gpio_trig error\n");
			ret = -EINVAL;
			goto faile_device; 
		}
		
		printk("hc-sr04:gpio-trig:%d\n", hc_sr04.gpio_trig);

		ret = gpio_request(hc_sr04.gpio_trig, "hc-sr04 trig");
		
		if (ret != 0) {
				printk("hc-sr04:gpio-trig request error\n");
				goto faile_gpio_trig_request;
		}
		
		
		ret = gpio_direction_output(hc_sr04.gpio_trig, 0); //默认高电平led熄灭
		if (ret != 0) {
			printk("hc_sr04.gpio_trig set direction_output error\n");
			ret = -EINVAL;
			goto faile_gpio_direction;
		}

		ret = gpio_direction_input(hc_sr04.gpio_echo);
		if (ret != 0) {
			printk("hc_sr04.gpio_echo set direction_input error\n");
			ret = -EINVAL;
			goto faile_gpio_direction;
		}
		printk("hc_sr04 probe successful\n");

		hc_sr04.irq_echo = gpio_to_irq(hc_sr04.gpio_echo);
	    if (hc_sr04.irq_echo < 0) {
	        dev_err(&dev->dev, "GPIO 转中断号失败\n");
	       goto faile_gpio_direction;
	    }
		
		ret = devm_request_irq(
			&dev->dev, 
			hc_sr04.irq_echo, 
			hc_sr04_irq_handler, 
			IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, 
			"hc_sr04_echo_irq", 
			&hc_sr04);
		if (ret != 0) {
			printk("devm_request_irq error\n");
			goto faile_gpio_direction;
		}
		INIT_WORK(&hc_sr04.work, hc_sr04_work_handler);
		init_waitqueue_head(&hc_sr04.wq);
		return 0;	
	
	
	
	/* 出错释放设备，以前刚学c的时候就听说过goto有害论，但是内核这种用法还挺常见的，
	用来释放资源确实很不错，节省代码 */
	faile_gpio_direction:
		gpio_free(hc_sr04.gpio_trig);
	faile_gpio_trig_request:
		gpio_free(hc_sr04.gpio_echo);

	faile_device_nd:
		device_destroy(hc_sr04.class, hc_sr04.devid);
	faile_device:
		class_destroy(hc_sr04.class);	//删除/sys/devices/hc_sr04_drvs
	faile_class:
		cdev_del(&hc_sr04.cdev);			//删除设备
	faile_devid:
	faile_cdev:
		unregister_chrdev_region(hc_sr04.devid, HC_SR04_CNT);
		return ret;
	

}

static int hc_sr04_remove(struct platform_device *dev)
{
	printk("  hc_sr04 remove \n");
	printk("xxx exit\n");
	gpio_free(hc_sr04.gpio_echo);
	gpio_free(hc_sr04.gpio_trig);
    cdev_del(&hc_sr04.cdev);//删除设备	
	device_destroy(hc_sr04.class, hc_sr04.devid);//删除设备
	class_destroy(hc_sr04.class);	//删除class
	unregister_chrdev_region(hc_sr04.devid, HC_SR04_CNT);//释放设备号


	
	printk("  hc_sr04_remove end\n");
	return 0;
}

struct of_device_id hc_sr04_of_match[] = {
	{.compatible = "hjy,hc_sr04",},
	{/**/},

};


struct platform_driver  hc_sr04_drv = {
	.probe = hc_sr04_probe,
	.remove = hc_sr04_remove,
	.driver = {
		.name = "imx6ull-hc_sr04",/*无设备树用这个*/ 
		.of_match_table = hc_sr04_of_match, /*设备树匹配表*/
		
	},


};





/* 驱动入口函数 */
static int __init hc_sr04_drv_init(void)
{	
	platform_driver_register(&hc_sr04_drv);
	return 0;
    

}

/* 驱动出口函数 */
static void __exit hc_sr04_drv_exit(void)
{
	platform_driver_unregister(&hc_sr04_drv);
}

/*注册驱动和卸载驱动入口函数*/
module_init(hc_sr04_drv_init);
module_exit(hc_sr04_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hjy");


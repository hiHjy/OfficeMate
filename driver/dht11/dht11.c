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
#define TIMEOUT 200000

#define DHT11_CNT 1
#define DHT11_NAME "dht11"


/*dht11 设备结构体*/
static struct dht11 {
	dev_t devid; //设备号
	struct cdev cdev; //字符设备核心结构体
	struct class *class;
	struct device *device;
	struct device_node *nd; /*设备节点*/
	
	int gpio;
	
} dht11;


static int dht11_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &dht11;



	return 0;
}




static int dht11_release(struct inode *inode, struct file *filp) {

	struct dht11 *dev = (struct dht11 *)filp->private_data;

	return 0;
}
static int wait_level_timeout(int gpio, int level, u64 timeout) {
	u64 start = ktime_get_ns();
	while (gpio_get_value(gpio) != level) {
		cpu_relax(); //必用
		if (ktime_get_ns() - start > timeout) {
			
			printk("dht11 timeout\n");
			return -ETIMEDOUT;
		}
	}

	return 0;
}
static ssize_t dht11_read (struct file *filp, char __user *buf , size_t count , loff_t * arg)
{

	
	int ret = 0;
	u64 cur_time_ns;
	u64 high_start;
	u64 high_time;
	u64 timeout_start;
	struct dht11 *dev = (struct dht11 *)filp->private_data;
	gpio_direction_output(dht11.gpio, 1);   
	mdelay(2);
	gpio_set_value(dht11.gpio, 0);
	mdelay(20);
	gpio_set_value(dht11.gpio, 1);
	udelay(20);
	gpio_direction_input(dht11.gpio);       
	udelay(30);
	
	

	// while(gpio_get_value(dht11.gpio)) {
	// 	printk("等待gpio被拉低中---");
		
	// }

	if (wait_level_timeout(dht11.gpio, 0, TIMEOUT)) return -ETIMEDOUT; 
	if (wait_level_timeout(dht11.gpio, 1, TIMEOUT)) return -ETIMEDOUT; 
	if (wait_level_timeout(dht11.gpio, 0, TIMEOUT)) return -ETIMEDOUT; 
	//while(!(gpio_get_value(dht11.gpio) == 1)); 
	
	u8 data[5];
	int err = 0;
	memset(data,0, sizeof(data));
	int i, j;
	preempt_disable();
	for (i = 0; i < 5; i++) {
		
		for ( j = 7; j >= 0; j--) {
			
			if (wait_level_timeout(dht11.gpio, 0, TIMEOUT)) {
				err = -ETIMEDOUT;
				goto out;
				
			}
		
			if (wait_level_timeout(dht11.gpio, 1, TIMEOUT)){
				err = -ETIMEDOUT;
				goto out;
			} 

			high_start =  ktime_get_ns();
			if (wait_level_timeout(dht11.gpio, 0, TIMEOUT)) {
				
				err = -ETIMEDOUT;
				goto out;
			
			}

			high_time = ktime_get_ns() - high_start;
			
			
			if (high_time > 50000){
				data[i] |= (0x01 << j);
			}
			
			//while(gpio_get_value(dht11.gpio)
			
			
		}
	}
	
	preempt_enable();
		// u16 temp = data[0] << 8 | data[1];
		// u16 shidu = data[2] << 8 | data[3];
	
	//printk("温度:%d, 湿度%d\n", data[0], data[2]);
	u8 sum = (data[0] + data[1] + data[2] +data[3]) & 0xff;
	//printk("raw %u %u %u %u %u \n", data[0], data[1], data[2], data[3], data[4]);
	if (sum == data[4]) {
		if ((ret = copy_to_user(buf, data, sizeof(data)))) {
			return -EFAULT;
		} else {
			return sizeof(data);
		}

	}

	//mdelay(n)
	return 0;
out:
	preempt_enable();
	return err;
	
}

/* 
	设备文件操作集，应用层open/dev/xxx后，使用wrtie、open、close等系统函数时就会调用对应下面的函数，
	所以应用层调用read、write等系统调用的返回值什么的都是由下列函数自己确定的，
	而不是像打开一个普通的文件，read默认是阻塞的，你想返回什么你可以自己规定，
	应用层想正常使用就得按照你的要求来，逼格真高。
*/
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	
	.open = dht11_open,//open
	.release =  dht11_release,//close
	.read  = dht11_read,
};




static int dht11_probe(struct platform_device *dev) {
	
	
		
		printk("dht11_probe\n");
	
		/*	 1， 申请设备号		 */
		
		int ret = 0;
	
			//动态申请设备号
		ret = alloc_chrdev_region(&dht11.devid, 0, DHT11_CNT, DHT11_NAME);
		if (ret < 0) {
			printk("设备号申请失败\n");
			goto faile_devid;
		}
	
		/*	2，添加字符设备	  */
		
		dht11.cdev.owner = THIS_MODULE;
		
			//关键的一行，把文件操作集和设备文件绑定
		cdev_init(&dht11.cdev, &fops);
		ret = cdev_add(&dht11.cdev, dht11.devid, DHT11_CNT);
		if (ret < 0) {
			goto faile_cdev;
		}
	
		/*3, 创建设备节点*/
		
			//创建/sys/class/xxx
		dht11.class = class_create(THIS_MODULE, DHT11_NAME);
		if (IS_ERR(dht11.class)) {
			ret = PTR_ERR(dht11.class);
	
			goto faile_class;
		}
			//创建/dev/xxx
		dht11.device = device_create(dht11.class, NULL, dht11.devid, NULL, DHT11_NAME);
		if (IS_ERR(dht11.device)) {
			ret = PTR_ERR(dht11.class);
			goto faile_device;
		}
		
		/*
			关键部分，一定别忘了return 0，否则会执行下面的释放代码，然后会在卸载驱动的时候会再次释放，
			此时内核卡死，必须重启,我以前就踩过这个坑，好几百行代码里找不到错误
		  
		*/
		
		
		/*获取设备树的属性内容*/
			//现代驱动一般都是跟设备树结合，这里省略，后面会复习到
	
		dht11.nd = of_find_node_by_path("/dht11");
		if (dht11.nd == NULL) {
			printk("device node：dht11 not found\n");
			goto faile_device_nd;	
		}
		

		
		dht11.gpio = of_get_named_gpio(dht11.nd, "gpios", 0);
		if (dht11.gpio < 0) {
			printk("get dht11 gpio error\n");
			ret = -EINVAL;
			goto faile_device_nd; 
		}
		printk("dht11:gpio:%d\n", dht11.gpio);
		ret = gpio_request(dht11.gpio, "dht11");
		
		if (ret != 0) {
				printk("dht11:gpio request error\n");
				goto faile_request_gpio;
		}
		
		
				
		

		
		return 0;	
	
	
	
	/* 出错释放设备，以前刚学c的时候就听说过goto有害论，但是内核这种用法还挺常见的，
	用来释放资源确实很不错，节省代码 */


	faile_request_gpio:
		gpio_free(dht11.gpio);
	faile_device_nd:
		
	device_destroy(dht11.class, dht11.devid);	
	faile_device:
		
		class_destroy(dht11.class);	//删除/sys/devices/dht11_drvs
	faile_class:
		cdev_del(&dht11.cdev);			//删除设备
	faile_devid:
	faile_cdev:
		unregister_chrdev_region(dht11.devid, DHT11_CNT);
		return ret;
	

}

static int dht11_remove(struct platform_device *dev)
{
	printk("  dht11 remove \n");
	printk("xxx exit\n");
	gpio_free(dht11.gpio);

    cdev_del(&dht11.cdev);//删除设备	
	device_destroy(dht11.class, dht11.devid);//删除设备
	class_destroy(dht11.class);	//删除class
	unregister_chrdev_region(dht11.devid, DHT11_CNT);//释放设备号


	
	printk("  dht11_remove end\n");
	return 0;
}

struct of_device_id dht11_of_match[] = {
	{.compatible = "dht11-hjy",},
	{/**/},

};


struct platform_driver  dht11_drv = {
	.probe = dht11_probe,
	.remove = dht11_remove,
	.driver = {
		.name = "imx6ull-dht11",/*无设备树用这个*/ 
		.of_match_table = dht11_of_match, /*设备树匹配表*/
		
	},


};





/* 驱动入口函数 */
static int __init dht11_drv_init(void)
{	
	platform_driver_register(&dht11_drv);
	printk("dht11 init\n");
	return 0;
    

}

/* 驱动出口函数 */
static void __exit dht11_drv_exit(void)
{
	platform_driver_unregister(&dht11_drv);
	printk("dht11 exit\n");
}

/*注册驱动和卸载驱动入口函数*/
module_init(dht11_drv_init);
module_exit(dht11_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hjy");

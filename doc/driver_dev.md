# 驱动开发

本项目有两个传感器，摄像头的驱动内核自带，只需要内核启动UAC驱动即可，这里记录 HC-SR04（超声波传感器）和 DHT11（温湿度传感器）要点。

## HC-SR04

### 基本原理

先搞清楚这个传感器的基本工作原理：通过声波测距，发出的声音碰到障碍物会反弹，声音在空气里传播的速度是已知的，根据时间就能计算出测量的距离。具体就是：

1. 采用 IO 口 TRIG 触发测距， 给至少 10us 的高电平信号

2. 模块自动发送 8 个 40khz 的方波， 自动检测是否有信号返回

3. 有信号返回， 通过 IO 口 ECHO 输出一个高电平， 高电平持续的时间就是超声波从发射到返回的时间。

### 流程

1. 服务程序 500ms 周期性的去主动的read,从而去调用驱动的read，并触发传感器

   ```c
    	/* 启动触发信号 */
       gpio_set_value(hc_sr04.gpio_trig, 1);  
       udelay(15);
       gpio_set_value(hc_sr04.gpio_trig, 0);
   ```

2. 触发传感器后需阻塞等待IO口ECHO变高电平，这里用的中断和等待队列以及 工作线程配合

   ```c
   // 1. 先阻塞等待数据准备好
   
   wait_event_interruptible_timeout(dev->wq, dev->data_ready, msecs_to_jiffies(60)); //read 阻塞
   
   // 2. echo变高电平触发中断，进入中断，记录高电平开始的时间 hc_sr04_irq_handler
   
   	//开始时间
   dev->start_us = ktime_to_us(ktime_get()); 							//hc_sr04_irq_handler --第一次中断
   	//echo变低电平再次进入中断,记录高电平结束时间
   dev->end_us = ktime_to_us(ktime_get());									//hc_sr04_irq_handler --第二次中断
   	//得到高电平持续时间后进入工作线程处理
   schedule_work(&dev->work);															//hc_sr04_irq_handler --第二次中断
   
   // 3. 这里进入工作线程后计算距离
   
   	//计算距离
   u32 time_us = dev->end_us - dev->start_us;							 //hc_sr04_work_handler
   dev->distance_cm = time_us * 34 / 2000；									//hc_sr04_work_handler
   	//等待队列唤醒条件设为真
   dev->data_ready = 1;																		//hc_sr04_work_handler
   	//唤醒阻塞在read中的进程
   wake_up_interruptible(&dev->wq); 												//hc_sr04_work_handler
   
   // 4. 唤醒read中阻塞的进程将计算好的距离移交给应用层
   copy_to_user(buf, &dev->distance_cm, count)							//read 继续执行后续的代码
   
     
   
   ```

这是完整的一套流程，主要是先阻塞降低cpu占用，然后利用中断仅记录耗时较小的时间然后，计算等耗时操作都放到安全的中断下半部（struct work_struct）去执行，在工作线程函数中需要唤醒阻塞的进程，到此整套流程结束。

## DHT11

## 基本原理

按照时序要求，每触发一次，读4字节的数据即可。dht11根据返回高电平的时长来发送0或1。

## 流程

较为简单，只要触发一次采集按照要求读四个字节即可关键是需要严格的按照时序要求去读。这里封装了一个工具函数，用于dht11等待io口变为某种状态。

```c
static int wait_level_timeout(int gpio, int level, u64 timeout) {
	u64 start = ktime_get_ns();
	while (gpio_get_value(gpio) != level) {
		cpu_relax(); //必用，告诉CPU当前处于忙等待循环，可以适当降低功耗
		if (ktime_get_ns() - start > timeout) {
			
			printk("dht11 timeout\n");
			return -ETIMEDOUT;
		}
	}

	return 0;
}
```

1. 触发采集

   ```c
   gpio_direction_output(dht11.gpio, 1);   //io口初始高电平
   mdelay(2);
   gpio_set_value(dht11.gpio, 0);					//拉低
   mdelay(20);
   gpio_set_value(dht11.gpio, 1);					//拉高
   udelay(20);
   gpio_direction_input(dht11.gpio);      	//io口的电平由dht11控制
   ```

2. 步骤1的作用是触发dht11的采集，如果正常，dht11一定会给予回应，步骤1的结束数据线是高电平，我们需要检测数据线是否被dht11拉低。如果没问题，dht11会拉低数据线，如果没有，可能是系统负载太高，或者就是dht11这个模块坏了。

   ```c
   wait_level_timeout(dht11.gpio, 0, TIMEOUT) 
   ```

   

3. 如果步骤2没问题，至少说明dht11模块没问题，那么按照时序要求，去检测相应的高低电平变化。

   ```c
   if (wait_level_timeout(dht11.gpio, 1, TIMEOUT)){
   		err = -ETIMEDOUT;
   		goto out;
   				
   } 
   if (wait_level_timeout(dht11.gpio, 0, TIMEOUT)){
   	err = -ETIMEDOUT;
   	goto out;
   				
   }
   ```

   如果有任何一步超时说明系统当前负载可能过高（实测验证过，当负载高时会超时严重)，当超时发生是此时不应该去读数据了，因为肯定是不对的。

> 步骤2和3其实就算是dht11的开始发数据的信号，为什么代码要这么写？目的就是要与dht11同步，为接下来接收数据做准备？为什么这么麻烦？dht11用一个io口，它不像i2c，spi有固定的协议，也不像串口有个波特率来做到同步，只能通过这种方式来实现主从的通讯。

4. 读取数据，并拷贝给应用层。

   ```c
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
   
   			high_start =  ktime_get_ns(); //高电平开始时间
   			if (wait_level_timeout(dht11.gpio, 0, TIMEOUT)) {
   				
   				err = -ETIMEDOUT;
   				goto out;
   			
   			}
   
   			high_time = ktime_get_ns() - high_start; //高电平持续时间
   			
   			
   			if (high_time > 50000){						
   				data[i] |= (0x01 << j);
   			}
   			
   			
   			
   		}
   	}
   		
   	u8 sum = (data[0] + data[1] + data[2] +data[3]) & 0xff; //校验
   	//printk("raw %u %u %u %u %u \n", data[0], data[1], data[2], data[3], data[4]);
   	if (sum == data[4]) {
   		if ((ret = copy_to_user(buf, data, sizeof(data)))) {
   			return -EFAULT;
   		} else {
   			return sizeof(data);
   		}
   
   	}
   ```

   

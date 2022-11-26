#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/io.h>

#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/delay.h>

#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/semaphore.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Driver for CrossCorrelation");
#define DEVICE_NAME "cross_correlation"
#define DRIVER_NAME "cross_correlation"

//buffer size
#define BUFF_SIZE 1000

//addresses for registers
#define START_REG     0x00
#define TMPL_COLS_REG 0x04
#define TMPL_ROWS_REG 0x08
#define IMG_COLS_REG  0x0c
#define READY_REG     0x10
#define SUM2_REG 	  0x14
#define RST_REG       0x18

//*************************************************************************
static int cross_correlation_probe(struct platform_device *pdev);
static int cross_correlation_open(struct inode *i, struct file *f);
static int cross_correlation_close(struct inode *i, struct file *f);
static ssize_t cross_correlation_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t cross_correlation_write(struct file *f, const char __user *buf, size_t count, loff_t *off);
static int __init cross_correlation_init(void);
static void __exit cross_correlation_exit(void);
static int cross_correlation_remove(struct platform_device *pdev);

//*********************GLOBAL VARIABLES*************************************

unsigned int tmpl_cols, tmpl_rows, img_cols;
unsigned int ready, sum2;
int done;

DECLARE_WAIT_QUEUE_HEAD(readQ);
DECLARE_WAIT_QUEUE_HEAD(writeQ);
struct semaphore sem;

static struct file_operations my_fops =
{
    .owner = THIS_MODULE,
    .open = cross_correlation_open,
    .release = cross_correlation_close,
    .read = cross_correlation_read,
    .write = cross_correlation_write
};
static struct of_device_id device_of_match[] = {
	{ .compatible = "xlnx,xcorelation", },
	{ .compatible = "xlnx,xc_axi_bram_ctrl_0", }, //bram img
	{ .compatible = "xlnx,xc_axi_bram_ctrl_1", }, //bram tmpl
	{ .compatible = "xlnx,xc_axi_bram_ctrl_2", }, //bram resp
	{ .compatible = "xlnx,xc_axi_bram_ctrl_3", }, //bram sum1
	{ /* end of list */ },
};

static struct platform_driver my_driver = {
    .driver = {
	.name = DRIVER_NAME,
	.owner = THIS_MODULE,
	.of_match_table	= device_of_match,
    },
    .probe		= cross_correlation_probe,
    .remove	= cross_correlation_remove,
};

struct device_info {
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem *base_addr;
};

static struct device_info *cross_correlation = NULL;
static struct device_info *bram_img = NULL;
static struct device_info *bram_tmpl = NULL;
static struct device_info *bram_resp = NULL;
static struct device_info *bram_sum1 = NULL;

MODULE_DEVICE_TABLE(of, device_of_match);

static dev_t my_dev_id;
static struct class *my_class;
static struct cdev *my_cdev;

//***************************************************
// PROBE AND REMOVE
//***************************************************

int device_fsm = 0;

static int cross_correlation_probe(struct platform_device *pdev)
{
    struct resource *r_mem;
    int rc = 0;

    printk(KERN_INFO "Probing\n");

    r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!r_mem) {
	printk(KERN_ALERT "invalid address\n");
	return -ENODEV;
    }

	switch (device_fsm)
    {
		case 0: // device cross_correlation
		  cross_correlation = (struct device_info *) kmalloc(sizeof(struct device_info), GFP_KERNEL);
		  if (!cross_correlation)
			{
			  printk(KERN_ALERT "Cound not allocate cross_correlation device\n");
			  return -ENOMEM;
			}
		  cross_correlation->mem_start = r_mem->start;
		  cross_correlation->mem_end   = r_mem->end;
		  if(!request_mem_region(cross_correlation->mem_start, cross_correlation->mem_end - cross_correlation->mem_start+1, DRIVER_NAME))
			{
			  printk(KERN_ALERT "Couldn't lock memory region at %p\n",(void *)cross_correlation->mem_start);
			  rc = -EBUSY;
			  goto error1;
			}
		  cross_correlation->base_addr = ioremap(cross_correlation->mem_start, cross_correlation->mem_end - cross_correlation->mem_start + 1);
		  if (!cross_correlation->base_addr)
			{
			  printk(KERN_ALERT "[PROBE]: Could not allocate cross_correlation iomem\n");
			  rc = -EIO;
			  goto error2;
			}
		  ++device_fsm;
		  printk(KERN_INFO "[PROBE]: Finished probing cross_correlation.\n");
		  return 0;
		  error2:
			release_mem_region(cross_correlation->mem_start, cross_correlation->mem_end - cross_correlation->mem_start + 1);
		  error1:
			return rc;
		  break;

		case 1: // device bram_img
		  bram_img = (struct device_info *) kmalloc(sizeof(struct device_info), GFP_KERNEL);
		  if (!bram_img)
			{
			  printk(KERN_ALERT "Cound not allocate bram_img device\n");
			  return -ENOMEM;
			}
		  bram_img->mem_start = r_mem->start;
		  bram_img->mem_end   = r_mem->end;
		  if(!request_mem_region(bram_img->mem_start, bram_img->mem_end - bram_img->mem_start+1, DRIVER_NAME))
			{
			  printk(KERN_ALERT "Couldn't lock memory region at %p\n",(void *)bram_img->mem_start);
			  rc = -EBUSY;
			  goto error3;
			}
		  bram_img->base_addr = ioremap(bram_img->mem_start, bram_img->mem_end - bram_img->mem_start + 1);
		  if (!bram_img->base_addr)
			{
			  printk(KERN_ALERT "[PROBE]: Could not allocate bram_img iomem\n");
			  rc = -EIO;
			  goto error4;
			}
		  ++device_fsm;
		  printk(KERN_INFO "[PROBE]: Finished probing bram_img.\n");
		  return 0;
		  error4:
			release_mem_region(bram_img->mem_start, bram_img->mem_end - bram_img->mem_start + 1);
		  error3:
			return rc;
		  break;

		case 2: // device bram_tmpl
		  bram_tmpl = (struct device_info *) kmalloc(sizeof(struct device_info), GFP_KERNEL);
		  if (!bram_tmpl)
			{
			  printk(KERN_ALERT "Cound not allocate bram_tmpl device\n");
			  return -ENOMEM;
			}
		  bram_tmpl->mem_start = r_mem->start;
		  bram_tmpl->mem_end   = r_mem->end;
		  if(!request_mem_region(bram_tmpl->mem_start, bram_tmpl->mem_end - bram_tmpl->mem_start+1, DRIVER_NAME))
			{
			  printk(KERN_ALERT "Couldn't lock memory region at %p\n",(void *)bram_tmpl->mem_start);
			  rc = -EBUSY;
			  goto error5;
			}
		  bram_tmpl->base_addr = ioremap(bram_tmpl->mem_start, bram_tmpl->mem_end - bram_tmpl->mem_start + 1);
		  if (!bram_tmpl->base_addr)
			{
			  printk(KERN_ALERT "[PROBE]: Could not allocate bram_tmpl iomem\n");
			  rc = -EIO;
			  goto error6;
			}
		  ++device_fsm;
		  printk(KERN_INFO "[PROBE]: Finished probing bram_tmpl.\n");
		  return 0;
		  error6:
			release_mem_region(bram_tmpl->mem_start, bram_tmpl->mem_end - bram_tmpl->mem_start + 1);
		  error5:
			return rc;
		  break;
				
		case 3: // device bram_resp
		  bram_resp = (struct device_info *) kmalloc(sizeof(struct device_info), GFP_KERNEL);
		  if (!bram_resp)
			{
			  printk(KERN_ALERT "Cound not allocate bram_resp device\n");
			  return -ENOMEM;
			}
		  bram_resp->mem_start = r_mem->start;
		  bram_resp->mem_end   = r_mem->end;
		  if(!request_mem_region(bram_resp->mem_start, bram_resp->mem_end - bram_resp->mem_start+1, DRIVER_NAME))
			{
			  printk(KERN_ALERT "Couldn't lock memory region at %p\n",(void *)bram_resp->mem_start);
			  rc = -EBUSY;
			  goto error7;
			}
		  bram_resp->base_addr = ioremap(bram_resp->mem_start, bram_resp->mem_end - bram_resp->mem_start + 1);
		  if (!bram_resp->base_addr)
			{
			  printk(KERN_ALERT "[PROBE]: Could not allocate bram_resp iomem\n");
			  rc = -EIO;
			  goto error8;
			}
		  ++device_fsm;
		   printk(KERN_INFO "[PROBE]: Finished probing bram_resp.\n");
		  return 0;
		  error8:
			release_mem_region(bram_resp->mem_start, bram_resp->mem_end - bram_resp->mem_start + 1);
		  error7:
			return rc;
		  break;
						
		case 4: // device bram_sum1
				  
			  bram_sum1 = (struct device_info *) kmalloc(sizeof(struct device_info), GFP_KERNEL);
			  if (!bram_sum1)
				{
				  printk(KERN_ALERT "Cound not allocate bram_sum1 device\n");
				  return -ENOMEM;
				}
			  bram_sum1->mem_start = r_mem->start;
			  bram_sum1->mem_end   = r_mem->end;
			  if(!request_mem_region(bram_sum1->mem_start, bram_sum1->mem_end - bram_sum1->mem_start+1, DRIVER_NAME))
				{
				  printk(KERN_ALERT "Couldn't lock memory region at %p\n",(void *)bram_sum1->mem_start);
				  rc = -EBUSY;
				  goto error9;
				}
			  bram_sum1->base_addr = ioremap(bram_sum1->mem_start, bram_sum1->mem_end - bram_sum1->mem_start + 1);
			  if (!bram_sum1->base_addr)
				{
				  printk(KERN_ALERT "[PROBE]: Could not allocate bram_sum1 iomem\n");
				  rc = -EIO;
				  goto error10;
				}
			  printk(KERN_INFO "[PROBE]: Finished probing bram_sum1.\n");
			  return 0;
			  error10:
				release_mem_region(bram_sum1->mem_start, bram_sum1->mem_end - bram_sum1->mem_start + 1);
			  error9:
				return rc;
			  break;

		default:
		  printk(KERN_INFO "[PROBE] Device FSM in illegal state. \n");
		  return -1;
		}
  printk(KERN_INFO "Succesfully probed driver\n");
  return 0;
}	
		
static int cross_correlation_remove(struct platform_device *pdev)
{
  switch (device_fsm)
    {
    case 0: //device cross_correlation
      printk(KERN_ALERT "cross_correlation device platform driver removed\n");
      iowrite32(0, cross_correlation->base_addr);
      iounmap(cross_correlation->base_addr);
      release_mem_region(cross_correlation->mem_start, cross_correlation->mem_end - cross_correlation->mem_start + 1);
      kfree(cross_correlation);
      break;

    case 1: //device bram_img
      printk(KERN_ALERT "bram_img platform driver removed\n");
      iowrite32(0, bram_img->base_addr);
      iounmap(bram_img->base_addr);
      release_mem_region(bram_img->mem_start, bram_img->mem_end - bram_img->mem_start + 1);
      kfree(bram_img);
      --device_fsm;
      break;

    case 2: //device bram_tmpl
      printk(KERN_ALERT "bram_tmpl platform driver removed\n");
      iowrite32(0, bram_tmpl->base_addr);
      iounmap(bram_tmpl->base_addr);
      release_mem_region(bram_tmpl->mem_start, bram_tmpl->mem_end - bram_tmpl->mem_start + 1);
      kfree(bram_tmpl);
      --device_fsm;
      break;
	  
	case 3: //device bram_resp
      printk(KERN_ALERT "bram_resp platform driver removed\n");
      iowrite32(0, bram_resp->base_addr);
      iounmap(bram_resp->base_addr);
      release_mem_region(bram_resp->mem_start, bram_resp->mem_end - bram_resp->mem_start + 1);
      kfree(bram_resp);
      --device_fsm;
      break;
	  
	  
    case 4: //device bram_sum1
      printk(KERN_ALERT "bram_sum1 device platform driver removed\n");
      iowrite32(0,bram_sum1->base_addr);
      iounmap(bram_sum1->base_addr);
      release_mem_region(bram_sum1->mem_start,bram_sum1->mem_end - bram_sum1->mem_start + 1);
      kfree(bram_sum1);
      --device_fsm;
      break;

    default:
      printk(KERN_INFO "[REMOVE] Device FSM in illegal state. \n");
      return -1;
    }
  printk(KERN_INFO "Succesfully removed driver\n");
  return 0;
}		


//***************************************************
// OPEN & CLOSE
//***************************************************

static int cross_correlation_open(struct inode *i, struct file *f)
{
    //printk("cross_correlation opened\n");
    return 0;
}
static int cross_correlation_close(struct inode *i, struct file *f)
{
    //printk("cross_correlation closed\n");
    return 0;
}

//***************************************************
// READ & WRITE
//***************************************************

int end_read = 0;
unsigned int resp_cnt = 0;
unsigned int sum1_cnt = 0;

ssize_t cross_correlation_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
  char buf[BUFF_SIZE];
  long int len=0;
  int val;
  int minor = MINOR(pfile->f_inode->i_rdev);

  /* printk(KERN_INFO "cross_correlation READ entered \n"); */
  //printk(KERN_INFO "len = %ld, end_read = %d\n", len, end_read);
  
  if (end_read == 1)
    {
      end_read = 0;
      return 0;
    }

  if (down_interruptible(&sem))
	return -ERESTARTSYS;
  while (done == 0)   //Reading is not allowed until IP is started
  {
	up(&sem);
	if (wait_event_interruptible(readQ, (done == 1)))
		return 	-ERESTARTSYS;
	if (down_interruptible(&sem))
		return -ERESTARTSYS;
  }

  switch (minor)
    {
	case 0:  //device cross_correlation
		//printk(KERN_INFO "[READ] Succesfully read from cross_correlation device.\n");
		
		ready = ioread32(cross_correlation->base_addr + READY_REG);
		sum2 = ioread32(cross_correlation->base_addr + SUM2_REG);
		
		//printk(KERN_INFO "ready = %u, sum2 = %u\n", ready, sum2);
		len = scnprintf(buf, BUFF_SIZE, "ready = %u, sum2 = %u\n", ready, sum2);

		if (copy_to_user(buffer, buf, len))
		 return -EFAULT;
		end_read = 1;
		break;
		
    case 1: //device bram_img
		printk(KERN_INFO "[READ] Reading from bram_img device is not possible.\n");
		end_read = 1;
      break;

    case 2: //device bram_tmpl
		printk(KERN_INFO "[READ] Reading from bram_tmpl device is not possible.\n");
		end_read = 1;
      break;

	case 3: //device bram_resp
		if(resp_cnt < img_cols - tmpl_cols)
		{	
			val = ioread32(bram_resp->base_addr + 4*resp_cnt);
			
			if (resp_cnt == img_cols - tmpl_cols - 1)
				len = scnprintf(buf, BUFF_SIZE, "%d\n", val);
			else 
				len = scnprintf(buf, BUFF_SIZE, "%d ", val);
			
			resp_cnt++;
			if (copy_to_user(buffer, buf, len))
			{
			  printk(KERN_ERR "Copy to user does not work.\n");
			  return -EFAULT;
			}
		}
		else
		{
			end_read = 1;
			//printk(KERN_INFO "[READ] Succesfully read from bram_resp device.\n");
			resp_cnt = 0;
		}
	break;
	  
	case 4: //device bram_sum1
		if(sum1_cnt < img_cols - tmpl_cols) 
		{
			val = ioread32(bram_sum1->base_addr + sum1_cnt*4);
			
			if (resp_cnt == img_cols - tmpl_cols - 1)
				len = scnprintf(buf, BUFF_SIZE, "%d\n", val);
			else
				len = scnprintf(buf, BUFF_SIZE, "%d ", val);

			sum1_cnt++;
			if (copy_to_user(buffer, buf, len))
			{
			  printk(KERN_ERR "Copy to user does not work.\n");
			  return -EFAULT;
			}
		}
		else
		{
			end_read = 1;
			//printk(KERN_INFO "[READ] Succesfully read from bram_sum1 device.\n");
			sum1_cnt = 0;
		}
	break;
	  
    default:
      printk(KERN_ERR "[READ] Invalid minor. \n");
      end_read = 1;
    }

	up(&sem);
	wake_up_interruptible(&writeQ);
  
  return len;
}

ssize_t cross_correlation_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
  char buf[length+1];
  int minor = MINOR(pfile->f_inode->i_rdev);
  int pos=0;
  int val=0;
  unsigned int flag_init;
  
  if (copy_from_user(buf, buffer, length))
    return -EFAULT;
  buf[length]='\0';

  if (down_interruptible(&sem))
	return -ERESTARTSYS;
  while (ready == 0)
  {
	up(&sem);
	if (wait_event_interruptible(writeQ, (ready == 1)))
		return 	-ERESTARTSYS;
	if (down_interruptible(&sem))
		return -ERESTARTSYS;
  }
  
  switch (minor)
    {
    case 0: //device cross_correlation
      sscanf(buf, "%u, %u, %u, %u", &tmpl_cols, &tmpl_rows, &img_cols, &flag_init);
      //printk(KERN_INFO "[WRITE] %u, %u, %u, %u\n", tmpl_cols, tmpl_rows, img_cols, flag_init);

	  if (flag_init == 1)
	  {
		  // ===================
		  // Initialization
		  // ===================
		  done = 0; //Reading is not possible anymore	
		  
		  iowrite32(1, cross_correlation->base_addr + RST_REG);
		  udelay(1000);
		  udelay(1000);
		  udelay(1000);
		  iowrite32(0, cross_correlation->base_addr + RST_REG);
		  udelay(1000);
		  udelay(1000);
		  
		  iowrite32(tmpl_cols, cross_correlation->base_addr + TMPL_COLS_REG);
		  iowrite32(tmpl_rows, cross_correlation->base_addr + TMPL_ROWS_REG);
		  iowrite32(img_cols, cross_correlation->base_addr + IMG_COLS_REG);
		  
		  up(&sem);
		  //printk(KERN_INFO "[WRITE] Succesfully initialized cross_correlation device. %u, %u, %u\n", tmpl_cols, tmpl_rows, img_cols);
	  }
      else if(flag_init == 0)
      {
		  // ===================
		  // Start 
		  // ===================
		  
		  iowrite32(1, cross_correlation->base_addr + START_REG);
		  udelay(1000);
		  while (ioread32(cross_correlation->base_addr + READY_REG))
		    udelay(100);
		  iowrite32(0, cross_correlation->base_addr + START_REG);	  
		  
		  ready = 0; //Writing is not possible anymore
		  done = 1; //Reading is possible 
		  up(&sem);
		  wake_up_interruptible(&readQ);
		  
		  //printk(KERN_INFO "[WRITE] Succesfully started cross_correlation device\n");

	  }
	  break;
    case 1: //device bram_img
      sscanf(buf, "%d, %d", &pos, &val);
      iowrite32(val, bram_img->base_addr + 4*pos);
      //printk(KERN_INFO "[WRITE] Succesfully wrote into bram_img device. pos = %d, val = %d\n", pos, val);
      up(&sem);
	  break;

    case 2: //device bram_tmpl
      sscanf(buf, "%d, %d", &pos, &val);
      iowrite32(val, bram_tmpl->base_addr + 4*pos);    
      //printk(KERN_INFO "[WRITE] Succesfully wrote into bram_tmpl device. pos = %d, val = %d\n", pos, val);
      up(&sem);
	  break;

	case 3: //device bram_resp
      printk(KERN_INFO "[WRITE] Writing into bram_resp device is not possible.\n");
      up(&sem);
	  break;

    case 4: //device bram_sum1   
      printk(KERN_INFO "[WRITE] Writing into bram_sum1 device is not possible.\n");
      up(&sem);
	  break;
	  
    default:
      up(&sem);
	  printk(KERN_INFO "[WRITE] Invalid minor. \n");
  }
  
  return length;
}

//***************************************************
// INIT & EXIT
//***************************************************

static int __init cross_correlation_init(void)
{
   printk(KERN_INFO "\n");
   printk(KERN_INFO "cross_correlation driver starting insmod.\n");

   ready = 1;
   done = 0;
	
   //Init semaphore
   sema_init(&sem,1);
   
   if (alloc_chrdev_region(&my_dev_id, 0, 5, "cross_correlation_region") < 0){
      printk(KERN_ERR "failed to register char device\n");
      return -1;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "cross_correlation_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id),0), NULL, "xlnx,cross_correlation") == NULL) {
      printk(KERN_ERR "failed to create device cross_correlation\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created - cross_correlation\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id),1), NULL, "xlnx,bram_img") == NULL) {
     printk(KERN_ERR "failed to create device bram_img\n");
     goto fail_2;
   }
   printk(KERN_INFO "device created - bram_img\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id),2), NULL, "xlnx,bram_tmpl") == NULL) {
     printk(KERN_ERR "failed to create device bram_tmpl\n");
     goto fail_3;
   }
   printk(KERN_INFO "device created - bram_tmpl\n");

    if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id),3), NULL, "xlnx,bram_resp") == NULL) {
     printk(KERN_ERR "failed to create device bram_resp\n");
     goto fail_4;
    }
    printk(KERN_INFO "device created - bram_resp\n");

	if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id),4), NULL, "xlnx,bram_sum1") == NULL) {
     printk(KERN_ERR "failed to create device bram_sum1\n");
     goto fail_5;
   }
   printk(KERN_INFO "device created - bram_sum1\n");
   
	my_cdev = cdev_alloc();
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;

	if (cdev_add(my_cdev, my_dev_id, 5) == -1)
	{
      printk(KERN_ERR "failed to add cdev\n");
      goto fail_6;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "cross_correlation driver initialized.\n");

   return platform_driver_register(&my_driver);

   fail_6:
     device_destroy(my_class, MKDEV(MAJOR(my_dev_id),4));
   fail_5:
     device_destroy(my_class, MKDEV(MAJOR(my_dev_id),3));
   fail_4:
     device_destroy(my_class, MKDEV(MAJOR(my_dev_id),2));
   fail_3:
     device_destroy(my_class, MKDEV(MAJOR(my_dev_id),1));
   fail_2:
     device_destroy(my_class, MKDEV(MAJOR(my_dev_id),0));
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit cross_correlation_exit(void)
{
	printk(KERN_INFO "cross_correlation driver starting rmmod.\n");
	platform_driver_unregister(&my_driver);
	cdev_del(my_cdev);

	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),4));  
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),3));	
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),2));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),1));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),0));
	class_destroy(my_class);
	unregister_chrdev_region(my_dev_id,1);
	printk(KERN_INFO "cross_correlation driver exited.\n");
}

module_init(cross_correlation_init);
module_exit(cross_correlation_exit);

MODULE_AUTHOR ("CrossBubbles Team");
MODULE_DESCRIPTION("Test Driver for cross_correlation output.");
MODULE_ALIAS("custom:cross_correlation");

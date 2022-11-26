#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/io.h>

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

//max array size
#define IMG_SIZE 160000
#define TMPL_SIZE 51000
#define RESULT_SIZE 800

//buffer size
#define BUFF_SIZE 100

//addresses for registers
#define START_REG     0x00
#define TMPL_COLS_REG 0x04
#define TMPL_ROWS_REG 0x08
#define IMG_COLS_REG  0x0c
#define READY_REG     0x10
#define SUM2_REG 	  0x14
#define RST_REG       0x18

//*************************************************************************
static int cross_correlation_open(struct inode *i, struct file *f);
static int cross_correlation_close(struct inode *i, struct file *f);
static void cross_correlation_start(void);
static ssize_t cross_correlation_read(struct file *f, char __user *buf, size_t len, loff_t *off);
static ssize_t cross_correlation_write(struct file *f, const char __user *buf, size_t count, loff_t *off);
static int __init cross_correlation_init(void);
static void __exit cross_correlation_exit(void);

//*********************GLOBAL VARIABLES************************************

//Variables and arrays for CROSS CORRELATION BRAMs and registers
int start;
int tmpl_cols, tmpl_rows, img_cols;

int img_array[IMG_SIZE];
int tmpl_array[TMPL_SIZE];

int resp_array[RESULT_SIZE];
int sum1_array[RESULT_SIZE];
int sum2;

int ready, reset;
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

//MODULE_DEVICE_TABLE(of, device_of_match);

static dev_t my_dev_id;
static struct class *my_class;
static struct cdev *my_cdev;

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
// CROSS_CORRELATION START 
//***************************************************
int j, m, k;
static void cross_correlation_start(void)
{
	/*********HARDWARE DESCRIPTION FROM SYSTEMC MODEL********/

	//Cross Correlation operation
	
	ready = 0;
		
	for (j = 0; j < img_cols - tmpl_cols; j++)
	{
		resp_array[j] = 0;
		sum1_array[j] = 0;
		sum2 = 0;
		
		//The distance is calculated for each point of original image (i, j) 
		
		for (k = 0; k < tmpl_rows; k++)
		{
			for (m = 0; m < tmpl_cols; m++)
			{	
				resp_array[j] += img_array[j + k * img_cols + m] * tmpl_array[k * tmpl_cols + m];
				sum1_array[j] += img_array[j + k * img_cols + m] * img_array[ j +  k * img_cols + m];
				sum2 += tmpl_array[k * tmpl_cols + m] * tmpl_array[k * tmpl_cols + m];
			   			
			}
		}

	}
	
	ready = 1;
	done = 1;

	up(&sem);
	wake_up_interruptible(&readQ);
	wake_up_interruptible(&writeQ);
	
	printk(KERN_INFO "Cross stripe done\n");
}

//***************************************************
// READ & WRITE
//***************************************************

int end_read = 0;
int resp_cnt = 0;
int sum1_cnt = 0;

static ssize_t cross_correlation_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
  char buf[BUFF_SIZE];
  long int len=0;
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
  while (done == 0)
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
		printk(KERN_INFO "[READ] Succesfully read from cross_correlation device.\n");
		
		len = scnprintf(buf, BUFF_SIZE, "ready = %d, sum2 = %d\n", ready, sum2);
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
			if (resp_cnt == img_cols - tmpl_cols - 1)
				len = scnprintf(buf, BUFF_SIZE, "%d\n", resp_array[resp_cnt]);
			else
				len = scnprintf(buf, BUFF_SIZE, "%d ", resp_array[resp_cnt]);
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
			printk(KERN_INFO "[READ] Succesfully read from bram_resp device.\n");
			resp_cnt = 0;
		}
	break;
	  
	case 4: //device bram_sum1
		if(sum1_cnt < img_cols - tmpl_cols) 
		{
			if (resp_cnt == img_cols - tmpl_cols - 1)
				len = scnprintf(buf, BUFF_SIZE, "%d\n", sum1_array[sum1_cnt]);
			else
				len = scnprintf(buf, BUFF_SIZE, "%d ", sum1_array[sum1_cnt]);
			
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
			printk(KERN_INFO "[READ] Succesfully read from bram_sum1 device.\n");
			sum1_cnt = 0;
		}
	break;
	  
    default:
      printk(KERN_ERR "[READ] Invalid minor. \n");
      end_read = 1;
    }

  up(&sem);

  return len;
}

static ssize_t cross_correlation_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
  char buf[length+1];
  int minor = MINOR(pfile->f_inode->i_rdev);
  int pos=0;
  int val=0;
  int flag_init;
  
  if (copy_from_user(buf, buffer, length))
    return -EFAULT;
  buf[length]='\0';

  if (down_interruptible(&sem))
	return -ERESTARTSYS;
  while (ready == 0)
  {
	up(&sem);
	if (wait_event_interruptible(readQ, (ready == 1)))
		return 	-ERESTARTSYS;
	if (down_interruptible(&sem))
		return -ERESTARTSYS;
  }
 
  switch (minor)
    {
    case 0: //device cross_correlation
      sscanf(buf, "%d, %d, %d, %d", &tmpl_cols, &tmpl_rows, &img_cols, &flag_init);
      printk(KERN_INFO "[WRITE] %d, %d, %d, %d\n", tmpl_cols, tmpl_rows, img_cols, flag_init);

	if (flag_init == 1)
	{
		  // ===================
		  // Initialization
		  // ===================
		
		  done = 0; //Reading is not possible anymore	  
		  printk(KERN_INFO "[WRITE] Succesfully initialized cross_correlation device. %d, %d, %d\n", tmpl_cols, tmpl_rows, img_cols);
		  
		  up(&sem);
	}
	else if(flag_init == 0)
        {
		  // ===================
		  // Reset and start 
		  // ===================
		  
		  //reset and start registers does not effect system in software version of cross correlation
		  reset = 1;
		  reset = 0;
		  start = 1;
		  
		  cross_correlation_start();
		  
		  printk(KERN_INFO "[WRITE] Succesfully started cross_correlation device\n");
	 }
	 break;
    case 1: //device bram_img
      sscanf(buf, "%d, %d", &pos, &val);
      img_array[pos] = val;
      printk(KERN_INFO "[WRITE] Succesfully wrote into bram_img device. pos = %d, val = %d\n", pos, img_array[pos]);
      
      up(&sem);
      break;

    case 2: //device bram_tmpl
      sscanf(buf, "%d, %d", &pos, &val);
      tmpl_array[pos] = val;
     printk(KERN_INFO "[WRITE] Succesfully wrote into bram_tmpl device. pos = %d, val = %d\n", pos, tmpl_array[pos]);
      
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
      printk(KERN_INFO "[WRITE] Invalid minor. \n");
      up(&sem);
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

   //Init cross correlation arrays (BRAMs) and registers
	int i;

	for (i = 0; i < IMG_SIZE; i++)
		img_array[i] = 0;

	for (i = 0; i < TMPL_SIZE; i++)
		tmpl_array[i] = 0;

	for (i = 0; i < RESULT_SIZE; i++)
		resp_array[i] = 0;

	for (i = 0; i < RESULT_SIZE; i++)
		sum1_array[i] = 0;

	start = 0;
	tmpl_cols = 0;
	tmpl_rows = 0;
	img_cols = 0;
	sum2 = 0;
	ready = 1; 
	reset = 0;

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

   return 0;

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
	//platform_driver_unregister(&my_driver);
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

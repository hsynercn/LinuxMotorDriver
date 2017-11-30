#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <uapi/asm-generic/errno-base.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/time.h>

#define MxINA_PIN_NUMBER 66
#define MxINB_PIN_NUMBER 69
#define MxEN_DIAG_PIN_NUMBER 45

#define DEVICE_NUMBER 1
#define BUF_SIZE      512
#define DEVICE_NAME   "polulu_pin_control_group_1"

#define CLKWE            _IO('q', 1)
#define CLKWD            _IO('q', 2)
#define CCLKWE            _IO('q', 3)
#define CCLKWD            _IO('q', 4)
#define MOTORE            _IO('q', 5)
#define MOTORD            _IO('q', 6)




struct motor_support_dev {

    struct cdev cdev;
    struct gpio MxINA;
	struct gpio MxINB;
	struct gpio MxEN_DIAG;
    int stateMxINA;
	int stateMxINB;
	int stateMxEN_DIAG;

};



static struct motor_support_dev *motor_support_devp;
static dev_t deviceNumber;
static struct class *motor_support_classp;

static ssize_t motor_read(struct file* F, char *buf, size_t count, loff_t *f_pos);

static ssize_t motor_write(struct file* F, const char *buf, size_t count, loff_t *f_pos);

static int motor_open(struct inode *inode, struct file *file);

static int motor_close(struct inode *inode, struct file *file);

static int motor_ioctl(struct file *f, unsigned int command );

static struct file_operations motor_fileops =
{
    .owner                = THIS_MODULE,
    .open                 = motor_open,
    .read                 = motor_read,
    .write                = motor_write,
    .release              = motor_close,
    .unlocked_ioctl       = motor_ioctl,
};

static ssize_t motor_read(struct file* F, char *buf, size_t count, loff_t *f_pos)
{
    return -EPERM;
}

static ssize_t motor_write(struct file* F, const char *buf, size_t count, loff_t *f_pos)
{
    return -EPERM;
}

static int motor_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int motor_close(struct inode *inode, struct file *file)
{
    return 0;
}



static int motor_ioctl(struct file *f, unsigned int command )
{
    int retval = 1;


    switch (command)
    {

        case CLKWE:
			printk("CLKWE\n");
		    if(motor_support_devp->stateMxINB == 1)
			{
				gpio_set_value( motor_support_devp->MxINB.gpio , 0 );
				motor_support_devp->stateMxINB = 0;
			}

		    gpio_set_value( motor_support_devp->MxINA.gpio , 1 );
			motor_support_devp->stateMxINA = 1;
			return retval;

        break;

		case CCLKWE:
			printk("CCLKWE\n");
			if(motor_support_devp->stateMxINA == 1)
			{
				gpio_set_value( motor_support_devp->MxINA.gpio , 0 );
				motor_support_devp->stateMxINA = 0;
			}

			gpio_set_value( motor_support_devp->MxINB.gpio , 1 );
			motor_support_devp->stateMxINB = 1;
			return retval;

		break;

		case CLKWD:
			printk("CLKWD\n");

		    gpio_set_value( motor_support_devp->MxINA.gpio , 0 );
			return retval;

		break;
		case CCLKWD:


		    gpio_set_value( motor_support_devp->MxINB.gpio , 0 );
			return retval;


		break;

		case MOTORE:
			printk("MOTORE\n");

		   	gpio_set_value( motor_support_devp->MxEN_DIAG.gpio , 1 );
			motor_support_devp->stateMxEN_DIAG = 1;
			return retval;


			break;

		case MOTORD:
			printk("MOTORD\n");

		   	gpio_set_value( motor_support_devp->MxEN_DIAG.gpio , 0 );
			motor_support_devp->stateMxEN_DIAG = 0;
			return retval;


		break;

        default:
        break;
    }

     return 0;
}





static int __init motor_support_init(void )
{
	int allocRet , gpioRequestReturn , cdevAddRet ;
	int requestA,requestB,requestEN;

	allocRet = alloc_chrdev_region( &deviceNumber , 0 , DEVICE_NUMBER , DEVICE_NAME );

	if( allocRet < 0 )
    {
		printk( KERN_ALERT "Device number allocation failure\n" );
		return -1;
    }

	printk( KERN_ALERT "Device number allocated\n" ) ;

	motor_support_classp = class_create( THIS_MODULE , DEVICE_NAME );

	if( motor_support_classp == NULL )
    {
		printk( KERN_ALERT "Class creation failure: %s\n",DEVICE_NAME  );
		unregister_chrdev_region( deviceNumber , DEVICE_NUMBER);

		return -EINVAL;

    }
	printk( KERN_ALERT "Class created: %s\n",DEVICE_NAME  );

	motor_support_devp = kmalloc( sizeof( struct motor_support_dev ) , GFP_KERNEL );

	if( motor_support_devp == 0 )
    {
		printk( KERN_ALERT "Kmalloc ( motor_support_devp ) failure\n" );

		unregister_chrdev_region( deviceNumber , DEVICE_NUMBER );

		class_destroy( motor_support_classp );

		return -ENOMEM;
    }



	requestA = gpio_request_one( MxINA_PIN_NUMBER , GPIOF_OUT_INIT_LOW , NULL );
	requestB = gpio_request_one( MxINB_PIN_NUMBER , GPIOF_OUT_INIT_LOW , NULL );
	requestEN = gpio_request_one( MxEN_DIAG_PIN_NUMBER , GPIOF_OUT_INIT_LOW , NULL );
	printk("A:%d B:%d EN:%d",requestA,requestB,requestEN);

    if( (requestA < 0) | (requestB < 0) | (requestEN < 0) )
    {
		printk( KERN_ALERT "motor_support_init pin request falilure\n" );
		if(requestA < 0)
		{
			printk(KERN_ALERT "requestA failed , PIN %d\n" ,MxINA_PIN_NUMBER );
		}
		else
		{
			gpio_free( MxINA_PIN_NUMBER );
		}
		if(requestB < 0)
		{
			printk(KERN_ALERT "requestB failed , PIN %d\n" ,MxINB_PIN_NUMBER );
		}
		else
		{
			gpio_free( MxINB_PIN_NUMBER );
		}
		if(requestEN < 0)
		{
			printk(KERN_ALERT "requestEN failed , PIN %d\n" , MxEN_DIAG_PIN_NUMBER );
		}
		else
		{
			gpio_free( MxEN_DIAG_PIN_NUMBER );
		}


		kfree( motor_support_devp );

		class_destroy( motor_support_classp );

		unregister_chrdev_region( deviceNumber , DEVICE_NUMBER );

		return -10;
    }

	printk( KERN_ALERT "Pins alloctaed\n" );

	motor_support_devp->MxINA.gpio = MxINA_PIN_NUMBER;
	motor_support_devp->MxINB.gpio = MxINB_PIN_NUMBER;
	motor_support_devp->MxEN_DIAG.gpio = MxEN_DIAG_PIN_NUMBER;

  	motor_support_devp->stateMxINA = 0;
	motor_support_devp->stateMxINB = 0;
	motor_support_devp->stateMxEN_DIAG = 0;


	printk( KERN_ALERT "Pins parameters set\n" );

	motor_support_devp->cdev.owner = THIS_MODULE;


    cdev_init( &motor_support_devp->cdev , &motor_fileops );


	printk( KERN_ALERT " cdev_add(  &motor_support_devp->cdev , deviceNumber , 1 )\n" );

	if( cdev_add(  &motor_support_devp->cdev , deviceNumber , 1 ) )
    {
		printk( KERN_ALERT "Char device add failure\n" );

		device_destroy( motor_support_classp , MKDEV( MAJOR( deviceNumber ) , MINOR( deviceNumber ) ) );
		printk( KERN_ALERT "Device destroyed\n" );

		gpio_set_value( motor_support_devp->MxINA.gpio , 0 );
		gpio_set_value( motor_support_devp->MxINB.gpio , 0 );
		gpio_set_value( motor_support_devp->MxEN_DIAG.gpio , 0 );

		gpio_free( motor_support_devp->MxINA.gpio );
		gpio_free( motor_support_devp->MxINB.gpio );
		gpio_free( motor_support_devp->MxEN_DIAG.gpio );


		kfree( motor_support_devp );
		printk( KERN_ALERT "motor_support_devp free\n" );

		class_destroy( motor_support_classp );
		printk( KERN_ALERT "class destoyed\n" );

		unregister_chrdev_region( deviceNumber , DEVICE_NUMBER );
		printk( KERN_ALERT "device number region released\n" );

		printk( KERN_ALERT "Resources are free\n"  );

		return -11;
    }

	printk( KERN_ALERT " cdev_add(  &motor_support_devp->cdev , deviceNumber , 1 ) SUCCEED\n" );

	if( device_create( motor_support_classp , NULL , MKDEV( MAJOR( deviceNumber ) , MINOR( deviceNumber ) ) , NULL , DEVICE_NAME 	) == NULL )
    {
		printk( KERN_ALERT "Device creation failure\n" );

		device_destroy( motor_support_classp , MKDEV( MAJOR( deviceNumber ) , MINOR( deviceNumber ) ) );

		gpio_free( motor_support_devp->MxINA.gpio );
		gpio_free( motor_support_devp->MxINB.gpio );
		gpio_free( motor_support_devp->MxEN_DIAG.gpio );

		kfree( motor_support_devp );

		class_destroy( motor_support_classp );

		unregister_chrdev_region( deviceNumber , DEVICE_NUMBER );

		return -47;

    }

	printk( KERN_ALERT "motor_support_init SUCCEED\n" );

	return 0;
}

static void __exit motor_support_exit( void )
{
	device_destroy( motor_support_classp , MKDEV( MAJOR( deviceNumber ) , MINOR( deviceNumber ) ) );

		gpio_free( motor_support_devp->MxINA.gpio );
		gpio_free( motor_support_devp->MxINB.gpio );
		gpio_free( motor_support_devp->MxEN_DIAG.gpio );

		kfree( motor_support_devp );

		class_destroy( motor_support_classp );

		unregister_chrdev_region( deviceNumber , DEVICE_NUMBER );
}


module_init( motor_support_init );
module_exit( motor_support_exit );

MODULE_LICENSE("GPL");

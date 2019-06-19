#include <linux/init.h>                 //  Macros used to mark up functions e.g. __init __exit{{{
#include <linux/module.h>               //  Core header for loading LKMs into the kernel
#include <linux/device.h>               //  Header to support the kernel Driver Model
#include <linux/kernel.h>               //  Contains types, macros, functions for the kernel
#include <linux/fs.h>                   //  Header for the Linux file system support
#include <linux/uaccess.h>              //  Required for the copy to user function
#include <linux/i2c.h>
#define  DEVICE_NAME "mpu9250_pslavkin" /// < The device will appear at /dev/mpu9250 using this value
#define  CLASS_NAME  "i2c"              /// < The device class -- this is a character device driver}}}
MODULE_LICENSE("GPL");                                                    /// < The license type -- this affects available functionality{{{
MODULE_AUTHOR("Pablo Slavkin");                                           /// < The author -- visible when you use modinfo
MODULE_DESCRIPTION("char device driver para acceder por i2C al MPU9250"); /// < The description -- see modinfo
MODULE_VERSION("0.1");                                                    /// < A version number to inform users}}}
static int    majorNumber;                  /// < Stores the device number -- determined automatically{{{
static char   message[256]          = {0};  /// < Memory for the string that is passed from userspace
static short  size_of_message;              /// < Used to remember the size of the string stored
static int    numberOpens           = 0;    /// < Counts the number of times the device is opened
static struct class*  mpu9250Class  = NULL; /// < The device-driver class struct pointer
static struct device* mpu9250Device = NULL; /// < The device-driver device struct pointer

// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open    ( struct inode * ,struct file *                  );
static int     dev_release ( struct inode * ,struct file *                  );
static ssize_t dev_read    ( struct file *  ,char *       ,size_t ,loff_t * );
static ssize_t dev_write   ( struct file *  ,const char * ,size_t ,loff_t * );
//=========================================================================
typedef enum {
   ACCEL=0,
   TEMP,
   GYRO,
   ACCEL_BARS,
   GYRO_BARS,
   ALL
} retrieveState_Enum;
static short int accelX,accelY,accelZ;
static short int gyroX,gyroY,gyroZ;
static short int temp;
static char barBuffer[21];
static retrieveState_Enum retrieveState=ALL;
struct i2c_client          *gClient;
const struct i2c_device_id *gId;

void horizonalBar(char* buffer, short int value, char Axe);
void readMpuData (void);/*}}}*/
static struct file_operations fops =/*{{{*/
{
/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
   .open    = dev_open,
   .read    = dev_read,
   .write   = dev_write,
   .release = dev_release,
};/*}}}*/
static int __init mpu9250_init(void) /*{{{*/
{
   printk(KERN_INFO "mpu9250: Initializing the mpu9250 LKM\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "mpu9250 failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "mpu9250: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   mpu9250Class = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(mpu9250Class)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(mpu9250Class);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "mpu9250: device class registered correctly\n");

   // Register the device driver
   mpu9250Device = device_create(mpu9250Class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(mpu9250Device)){               // Clean up if there is an error
      class_destroy(mpu9250Class);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(mpu9250Device);
   }
   printk(KERN_INFO "mpu9250: device class created correctly\n"); // Made it! device was initialized
   return 0;
}/*}}}*/
static void __exit mpu9250_exit(void) /*{{{*/
{
   device_destroy    ( mpu9250Class, MKDEV(majorNumber, 0           )); // remove the device
   class_unregister  ( mpu9250Class                                 ) ; // unregister the device class
   class_destroy     ( mpu9250Class                                 ) ; // remove the device class
   unregister_chrdev ( majorNumber, DEVICE_NAME                     ) ; // unregister the major number
   printk            ( KERN_INFO "mpu9250: Goodbye from the LKM!\n" ) ;
}/*}}}*/
static int dev_open(struct inode *inodep, struct file *filep) /*{{{*/
{
   numberOpens++;
   printk(KERN_INFO "mpu9250: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}/*}}}*/
static int dev_release(struct inode *inodep, struct file *filep)/*{{{*/
{
   printk(KERN_INFO "mpu9250: Device successfully closed\n");
   return 0;
}/*}}}*/
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)/*{{{*/
{
   int error_count = 0;
   readMpuData();
   switch (retrieveState) {
      case ACCEL:
         size_of_message=sprintf(message,"x=%7d y=%7d z=%7d\n",accelX,accelY,accelZ);
         break;
      case TEMP:
         size_of_message=sprintf(message,"temp=%7d\n",temp);
         break;
      case GYRO:
         size_of_message=sprintf(message,"x=%7d y=%7d z=%7d\n",gyroX,gyroY,gyroZ);
         break;
      case ACCEL_BARS: 
         horizonalBar(barBuffer,accelX,'X');
         size_of_message=sprintf(message,"%s",barBuffer);
         horizonalBar(barBuffer,accelY,'Y');
         size_of_message+=sprintf(message+size_of_message,"%s",barBuffer);
         horizonalBar(barBuffer,accelZ,'Z');
         size_of_message+=sprintf(message+size_of_message,"%s\n",barBuffer);
         break;
      case GYRO_BARS: 
         horizonalBar(barBuffer,gyroX,'X');
         size_of_message=sprintf(message,"%s",barBuffer);
         horizonalBar(barBuffer,gyroY,'Y');
         size_of_message+=sprintf(message+size_of_message,"%s",barBuffer);
         horizonalBar(barBuffer,gyroZ,'Z');
         size_of_message+=sprintf(message+size_of_message,"%s\n",barBuffer);
         break;
      case ALL:
      default:
         size_of_message  = sprintf(message                 ,"x=%7d y=%7d z=%7d\n" ,accelX ,accelY ,accelZ              );
         size_of_message += sprintf(message+size_of_message ,"temp=%7d\n"          ,temp                       );
         size_of_message += sprintf(message+size_of_message ,"x=%7d y=%7d z=%7d\n" ,gyroX  ,gyroY  ,gyroZ );
         break;
   }
   size_of_message++;
   error_count = copy_to_user(buffer, message, size_of_message);
   if (error_count!=0) {
      printk(KERN_INFO "mpu9250: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
   return size_of_message;
}/*}}}*/
void horizonalBar(char* buffer, short int value, char Axe)/*{{{*/
{
   int index, i;
   for(i=0;i<21;i++) {
      buffer[i]=' ';
   }
   buffer[10]=Axe;
   value/=(0x3FFF/10);
   if(value>10) value=10;
   else
      if(value<-10) value=-10;
   if(value<0) {
      value=-value;
      index=10-value;
   }
   else
      index=11;
   for(i=0;i<value;i++) {
      buffer[index+i]='-';
   }
}/*}}}*/
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) /*{{{*/
{
   int error_count=copy_from_user(message,buffer,len);
   if (error_count!=0) {
      printk(KERN_INFO "mpu9250: Failed to receive %d characters from the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "mpu9250: Received %zu characters from the user\n", len);
   sscanf(message,"%d",(int*)&retrieveState);
   printk("mpu9250: nuevo estado:%d\n",retrieveState);
   return len;
}/*}}}*/
void readMpuData (void)/*{{{*/
{
   int rv;
   char buf[20];
   char reg = 0x3B;
   rv       = i2c_master_send ( gClient ,&reg ,1  );
   rv       = i2c_master_recv ( gClient ,buf  ,14 );
   accelX   = (short int)( buf[ 0  ] * 256 + buf[ 1  ]);
   accelY   = (short int)( buf[ 2  ] * 256 + buf[ 3  ]);
   accelZ   = (short int)( buf[ 4  ] * 256 + buf[ 5  ]);
   temp     = (short int)( buf[ 6  ] * 256 + buf[ 7  ]);
   gyroX    = (short int)( buf[ 8  ] * 256 + buf[ 9  ]);
   gyroY    = (short int)( buf[ 10 ] * 256 + buf[ 11 ]);
   gyroZ    = (short int)( buf[ 12 ] * 256 + buf[ 13 ]);
   temp=temp/333+21; //segun datasheet
}/*}}}*/
static const struct i2c_device_id mpu9250_pslavkin_i2c_id[] = {/*{{{*/
    { "mpu9250_pslavkin", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, mpu9250_pslavkin_i2c_id);

static const struct of_device_id mpu9250_pslavkin_of_match[] = {
    { .compatible = "mse,mpu9250_pslavkin" },
    { }
};
MODULE_DEVICE_TABLE(of, mpu9250_pslavkin_of_match);

static int mpu9250_pslavkin_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
   gClient = client;
   gId     = id;
   pr_info("mpu9250_pslavkin_insert!\n");
   mpu9250_init ( );
   return 0;
}

static int mpu9250_pslavkin_remove(struct i2c_client *client)
{
    pr_info("mpu9250_pslavkin_remove!\n");
    mpu9250_exit();
    return 0;
}

static struct i2c_driver mpu9250_pslavkin_i2c_driver = {
    .driver = {
        .name           = "mpu9250_pslavkin",
        .of_match_table = mpu9250_pslavkin_of_match,
    },
    .probe    = mpu9250_pslavkin_probe,
    .remove   = mpu9250_pslavkin_remove,
    .id_table = mpu9250_pslavkin_i2c_id
};

module_i2c_driver(mpu9250_pslavkin_i2c_driver);/*}}}*/

/**************************************************************
* Class:  CSC-415-01 FALL 2020
* Name: Andrei Georgescu
* Student ID: 920776919
* Project: Assignment 6 – Device Driver
*
* File: driver_module.c
*
* Description: This is a skeleton device driver.
*              It is responsible for recieving a string from a user
*              application and then returning the amount of words
*              within that string back to the user application.
*
**************************************************************/

#include <linux/init.h>             // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/device.h>           // Header to support the kernel Driver Model
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
#include <linux/fs.h>               // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/ioctl.h>            // Required for IOCTL functions

#define  DEVICE_NAME "cscdevchar"   /// Device will appear as /dev/cscdevchar
#define  CLASS_NAME  "cscdev"       /// Device class name (character device driver)

// Defining IOCTL
#define  WR_DATA _IOW('a', 'a', int32_t*)
#define  RD_DATA _IOR('a', 'a', int32_t*)
int32_t  value = 0;

MODULE_LICENSE("GPL");              ///< The license type -- this affects available functionality
MODULE_AUTHOR("Andrei Georgescu");  ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Simple Linux char driver for CSC 415");  ///< The description -- see modinfo
MODULE_VERSION("0.0.1");            ///< A version number to inform users

static int    majorNumber;                     ///< Stores the device number -- determined automatically
static char   message[256] = { 0 };            ///< Memory for the string that is passed from userspace
static short  size_of_message;                 ///< Used to remember the size of the string stored
static int    numOfOpens = 0;                  ///< Counts the number of times the device is opened
static struct class*  cscdevcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* cscdevcharDevice = NULL; ///< The device-driver device struct pointer

// Prototype functions for the device driver
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long    etx_ioctl(struct file *, unsigned int cmd, unsigned long arg); // IOCTL prototype

/**
 * Device drivers are represented as a file structure in the kernel.
 * The file_operations structure is responsible for listing which functions are associated with
 * the device driver. It is common for char device drivers to implement open, read, write, and
 * release functions. In addition to this, this skeleton device driver also implements ioctl.
 *
 * NOTE: This uses a C99 syntax structure.
 */ 
static struct file_operations file_ops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .unlocked_ioctl = etx_ioctl,
    .release = dev_release,
};

static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch(cmd) {
        case WR_DATA: {         // When we write data, we copy FROM the user space
            copy_from_user(&value, (int32_t*) arg, sizeof(value));
            printk(KERN_INFO "WR_DATA: Value = %d\n", value);
            break;
        }
        case RD_DATA: {         // When we read data, we copy TO the user space
            copy_to_user((int32_t*) arg, &value, sizeof(value));
            break;
        }
    }
    return 0;
}

/** 
 * This is where the device driver kernel module is initialized.
 *
 * Static keyword is required for keeping this function only visible within this file.
 * The __init maco means that this function is only to be used at initialization time
 * and it will be discarded and have its memory freed after initialization.
 * 
 * @return 0 on success, anything else is considered to be an error.
 */ 
static int __init cscdevchar_init(void) {
    printk(KERN_INFO "CSC_DEV_CHAR: Initializing the cscdevchar Linux Kernel Module.\n");

    // Dynamically allocating a major number for the device.
    majorNumber = register_chrdev(0, DEVICE_NAME, &file_ops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "CSC_DEV_CHAR: Failed to allocated a major number.\n");
        return majorNumber;
    }
    printk(KERN_INFO "CSC_DEV_CHAR: Registered device with major number: %d\n", majorNumber);

    // Registering the device class.
    cscdevcharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(cscdevcharClass)) {
        // If an error occurred, clean up and exit.
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "CSC_DEV_CHAR: Failed to register device class.\n");
        return PTR_ERR(cscdevcharClass);
    }
    printk(KERN_INFO "CSC_DEV_CHAR: Device class successfully registered.\n");

    // Registering the device driver.
    cscdevcharDevice = device_create(cscdevcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(cscdevcharDevice)) {
        // If an error occurred, clean up and exit.
        class_destroy(cscdevcharClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "CSC_DEV_CHAR: Failed to create device.\n");
        return PTR_ERR(cscdevcharDevice);
    }
    printk(KERN_INFO "CSC_DEV_CHAR: Successfully created device class.\n"); // Success! device was initialized
    return 0;
}

/**
 * This is where the device driver is uninitalized and cleanup is performed.
 * 
 * The static and __macro are required for the same reasons they are in the
 * initialization function. 
 */ 
static void __exit cscdevchar_exit(void) {
    device_destroy(cscdevcharClass, MKDEV(majorNumber, 0));     // Removing the device
    class_unregister(cscdevcharClass);                          // Unregistering the device class
    class_destroy(cscdevcharClass);                             // Removing the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);                // Unregistering the major number
    printk(KERN_INFO "CSC_DEV_CHAR: Unitialized the cscdevchar Linux Kernel Module.\n");
}

/**
 * This function is called every time the device is opened from a user application.
 * 
 * Currently just keeps track of how many times the device has been opened and logs
 * that to the /var/log/syslog.
 * 
 * @param inodep A pointer to an inode object (defined in linux/fs.h)
 * @param filep A pointer to a file object (defined in linux/fs.h)
 */ 
static int dev_open(struct inode *inodep, struct file *filep) {
    numOfOpens++;
    printk(KERN_INFO "CSC_DEV_CHAR: Device has been opened %d time(s)\n", numOfOpens);
    return 0;
}

/**
 * This function is called everytime the device is being read from the user space. An example of 
 * this is when data is sent from the device and back to the user and it uses the copy_to_user()
 * function fo send a char buffer to the user space.
 * 
 * @param filep A pointer to the file object as defined in linux/fs.h
 * @param buffer Pointer to the buffer which this function writes to
 * @param len The length of the buffer
 * @param offset The offset of the buffer if one is needed.
 * 
 * @return 0 on success, anything else is considered to be an error.
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    // copy_to_user has the format ( * to, *from, size) and returns 0 on success
    error_count = copy_to_user(buffer, message, size_of_message);

    if (error_count == 0) {            // if true then have success
        printk(KERN_INFO "CSC_DEV_CHAR: Sent %d characters to the user\n", size_of_message);
        return (size_of_message = 0);  // clear the position to the start and return 0
    }
    else {
        printk(KERN_INFO "CSC_DEV_CHAR: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
    }
}

/**
 * This function is called when the device is being written to from the user space. An example of
 * this is when data is being sent to the device from the user.
 * 
 * In this case, the data is copied into the message array using sprintf().
 * 
 * @param filep A pointer to the file object as defined in linux/fs.h
 * @param buffer Pointer to the buffer which this function writes to
 * @param len The length of the buffer
 * @param offset The offset of the buffer if one is needed.
 */ 
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
    sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
    size_of_message = strlen(message);                  // store the length of the stored message
    printk(KERN_INFO "CSC_DEV_CHAR: Received %zu characters from the user space.\n", len);
    return len;
}

/**
 * This function is called whenever the device is released/closed by the user.
 * 
 * @param inodep A pointer to an inode object (defined in linux/fs.h)
 * @param filep A pointer to a file object (defined in linux/fs.h)
 */ 
static int dev_release(struct inode *inodep, struct file *filep){
    printk(KERN_INFO "CSC_DEV_CHAR: Device successfully closed.\n");
    return 0;
}

/**
 * These macros are from linux/init.h and are required to tell the program at insertion
 * time which functions are responsible for initializtion and cleanup.
 */ 
module_init(cscdevchar_init);
module_exit(cscdevchar_exit);
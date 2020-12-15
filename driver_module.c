#include <linux/init.h>
#include <linux/module.h>

#define DEVICE_NAME "415dchar"      // Device will be /dev/415dchar
#define CLASS_NAME "415d"           // Device class 

int driver_init(void) {
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
    return 0;
}

void driver_exit(void) {
    printk(KERN_ALERT "Inside the %s functino\n", __FUNCTION__);
}

module_init(driver_init);
module_exit(driver_exit);
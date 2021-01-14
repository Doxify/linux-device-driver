# Linux Device Driver

The purpose of this project was to create a simple device driver than can be loaded and ran in Linux. Device drivers are the key to how hardware devices interface with the computer.

This device driver has the minor functionality of taking a string from the user-space and returning how many words were in the string from the kernel-space.

## Build/Run Instructions
1. Clone the project
2. Go into the root directory that includes `Makerfile`.
3. Run `make` to build the device driver module and user-space test program.
4. Run `sudo insmod driver_module.ko` to install the device driver module.
5. Run `sudo ./driver_test` to launch the user-space program.
   1. To uninstall the device driver module, run `sudo rmmod driver_module.ko`

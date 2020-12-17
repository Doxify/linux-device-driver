/**************************************************************
* Class:  CSC-415-01 FALL 2020
* Name: Andrei Georgescu
* Student ID: 920776919
* Project: Assignment 6 – Device Driver
*
* File: driver_test.c
*
* Description: 
*
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Defining IOCTL
#define WR_DATA _IOW('a', 'a', int32_t*)
#define RD_DATA _IOR('a', 'a', int32_t*)

#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
// static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

int main(){
   int ret, fd;
   int32_t value;
   char stringToSend[BUFFER_LENGTH];
   
   // Opening the device with read/write access.
   printf("Starting user application to test the device driver...\n");
   fd = open("/dev/cscdevchar", O_RDWR);
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }

   // Prompting the user for input.
   printf("Enter a short string to send to the kernel module...\n");
   scanf("%[^\n]%*c", stringToSend); // Read in a string (with spaces)
   ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
   if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
   }

   // Reading from the driver.
   printf("Reading the data from the driver...\n");
   ioctl(fd, RD_DATA, (int32_t*) &value);
   printf("The inputted string (%s) has a length of %d...\n", stringToSend, value);

   // Closing the driver.
   printf("Closing the driver...\n");
   close(fd);

   return 0;
}
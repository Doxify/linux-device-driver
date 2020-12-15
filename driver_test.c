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

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

int main(){
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   
   // Opening the device with read/write access.
   printf("Starting device test code example...\n");
   fd = open("/dev/cscdevchar", O_RDWR);
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }

   // Prompting the user for input.
   printf("Type in a short string to send to the kernel module:\n");
   scanf("%[^\n]%*c", stringToSend); // Read in a string (with spaces)
   printf("Writing message to the device [%s].\n", stringToSend);
   ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
   if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
   }

   printf("Press ENTER to read back from the device...\n");
   getchar();

   // Reading response from the device linux kernel module.
   printf("Reading from the device...\n");
   ret = read(fd, receive, BUFFER_LENGTH);
   if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }

   printf("The received message is: [%s]\n", receive);
   printf("End of the program\n");

   return 0;
}
/**
 * @file   testebbchar.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the ebbchar.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/ebbchar.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<time.h>

#define DEVICE_NAME "/dev/mpu9250_pslavkin"    ///< The device will appear at /dev/ebbchar using this value
#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

struct timespec delay={0,100000000};

int main(){
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   printf("Starting device test code example...\r\n");
   fd = open(DEVICE_NAME, O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
//   printf("Type in a short string to send to the kernel module:\n");
//   scanf("%[^\n]%*c", stringToSend);                // Read in a string (with spaces)
//   printf("Writing message to the device [%s].\n", stringToSend);
//   ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
//   if (ret < 0){
//      perror("Failed to write the message to the device.");
//      return errno;
//   }
//
//   printf("Press ENTER to read back from the device...\n");
//   getchar();
//
   printf("Reading from the device...\n");
while(1) {
   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
   if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   printf("%s\n",receive);
   nanosleep(&delay,NULL);
}
 return 0;
}

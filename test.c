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
static char stringToSend[BUFFER_LENGTH];

struct timespec delay={0,100000000};

int main(){
   int ret, fd;
   printf("Starting device test code example...\r\n");
   fd = open(DEVICE_NAME, O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
   while(1) {
      printf("Type in a short string to send to the kernel module:\n");
      scanf("%s", stringToSend);                // Read in a string (with spaces)
      printf("Writing message to the device [%s].\n", stringToSend);
      ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
      if (ret < 0){
         perror("Failed to write the message to the device.");
         return errno;
      }
      printf("Reading from the device...\n");
      getchar();
      ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
      if (ret < 0){
         perror("Failed to read the message from the device.");
         return errno;
      }
      printf("%s\n",receive);
//      nanosleep(&delay,NULL);
   }
 return 0;
}

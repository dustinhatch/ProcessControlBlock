#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int main (void)
{

    int fda;
    int fdc; 
    int clientID; 
    
    struct values {
        char privateFIFO[12];
        int cpuBurst; 
        int memory; 
        int frag;
	      int numOfFrames; 
        int frames[11]; 
    } input;
    
    input.numOfFrames = 0; 
    input.frag = 0;
    
    struct response {
        int memory;
        int error; 
        int framesAssigned; 
        int frag; 
        int completion; 
    } output;
    
    memset(input.privateFIFO,0,12);

    clientID = getpid();
    sprintf(input.privateFIFO, "FIFO_%d", clientID); 

   
    if((fda=open("FIFO_to_server", O_WRONLY))<0)
        printf("cant open fifo to write");

    if ((mkfifo(input.privateFIFO,0666)<0 && errno != EEXIST)) {
        perror("cant create private_FIFO");
        exit(-1);
    }
  
    printf("Please enter CPU Burst: ");
    scanf("%d", &input.cpuBurst);
    
    printf("Please enter memory needed: ");
    scanf("%d", &input.memory);
    
    write(fda, &input, sizeof(input)); 
    
    if ((fdc = open(input.privateFIFO, O_RDONLY)) < 0)
        printf("Private_FIFO Error");
        
    read (fdc, &output, sizeof(output));
 
    if (output.memory == 0) {
        if (output.error == 1)
			      printf("The memory requested is greater than the server's total amount."); 
        else if (output.error == 2)
		        printf("The memory requested is greater than the amount currently available."); 
		}
    
    else
        printf("Client %d finished at %d with %d frames and fragmentation of %d in the last frame.", clientID, output.completion, output.framesAssigned, output.frag);
    
    close(fda); 
    close(fdc);
    unlink(input.privateFIFO);

    printf ("\nall done!\n");
}

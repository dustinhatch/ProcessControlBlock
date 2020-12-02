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

    int fda; // to write to server
    int fdc; // to read private response from server
    int clientID; // clientID for private_FIFO
    
    // struct definition
    struct values {
        char privateFIFO[12]; // private_FIFO name
        int cpuBurst; // CPU Burst Time
        int memory; // Memory needed
        int frag; // Sent to server as 0.
	    int numOfFrames; // Number of frames assigned to client.
        int frames[11]; // The frames assigned.
    } input;
    
    input.numOfFrames = 0; 
    input.frag = 0;
    
    struct response {
        int memory; // Memory allocated.
        int error; // Returns error.
        int framesAssigned; // The frame numbers assigned.
        int frag; // Fragmentation of last frame.
        int completion; // Time of completion.
    } output;
    
    memset(input.privateFIFO,0,12);

    clientID = getpid(); // creates clientID
    sprintf(input.privateFIFO, "FIFO_%d", clientID); // creates the private_FIFO name

    // opens FIFO_to_server for writing
    if((fda=open("FIFO_to_server", O_WRONLY))<0)
        printf("cant open fifo to write");

    // makes privateFIFO
    if ((mkfifo(input.privateFIFO,0666)<0 && errno != EEXIST)) {
        perror("cant create private_FIFO");
        exit(-1);
    }
  
    // gets array size
    printf("Please enter CPU Burst: ");
    scanf("%d", &input.cpuBurst);
    
    printf("Please enter memory needed: ");
    scanf("%d", &input.memory);
    
    write(fda, &input, sizeof(input)); // writes struct to the server.

    // opens privateFIFO for reading
    if ((fdc = open(input.privateFIFO, O_RDONLY)) < 0)
        printf("Private_FIFO Error");
    read (fdc, &output, sizeof(output)); // read result in private_FIFO from server
    
    if (output.memory == 0) {
        if (output.error == 1)
			 printf("The memory requested is greater than the server's total amount."); // Error 1 occurred. 
		 else if (output.error == 2)
			 printf("The memory requested is greater than the amount currently available."); //Error 2 occurred.
		}
    else
        printf("Client %d finished at %d with %d frames and fragmentation of %d in the last frame.", clientID, output.completion, output.framesAssigned, output.frag);
    close(fda); // closes write to server.
    close(fdc); // closes private_FIFO read to server.
    unlink(input.privateFIFO); // unlinks private FIFO to the server

    printf ("\nall done!\n");
    
}

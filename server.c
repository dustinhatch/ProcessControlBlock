#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

typedef struct pcb {  
    char privateFIFO[12];
	  int CPUBurst; 
	  int memory; 
    int frag; 
    int numOfFrames;
    int frames[11]; 
} PCB;

typedef struct node{  
	  struct pcb elements;
	  struct node *next;
} Node;

typedef struct queue{ 
	  Node *head;       
	  Node *tail;       
	  int sz;           
} Queue;

int size( Queue *Q ){
	  return Q->sz;
}

int isEmpty( Queue *Q ){
	  if( Q->sz == 0 ) 
        return 1;
        
	  return 0;
}

void enqueue( Queue *Q, struct pcb elements){
    Node *v = (Node*)malloc(sizeof(Node));
   
    if( !v ){
		    printf("ERROR: Insufficient memory\n");
		    return;
	  }
    
    v->elements = elements;
    v->next = NULL;
  
    if( isEmpty(Q) ) 
        Q->head = v;
	
    else 
        Q->tail->next = v;
	
    Q->tail = v;
	  Q->sz++;
}

PCB dequeue( Queue *Q ){
    PCB temp;
	  Node *oldHead;
    
	  if( isEmpty(Q) ) {
		    printf("ERROR: Queue is empty\n");
		    return temp;
	  }
    
	  oldHead = Q->head;
	  temp = Q->head->elements;
	  Q->head = Q->head->next;
	  free(oldHead);
	  Q->sz--;
	  return temp;
}

PCB first( Queue *Q ){
    PCB temp;
    
	  if( isEmpty(Q) ){
		    printf("ERROR: Queue is empty\n");
		    return temp;
	      }
        
	  printf("Head of the list is:%s\n", Q->head->elements.privateFIFO);
}

void destroyQueue( Queue *Q ){
    while( !isEmpty(Q) ) 
        dequeue(Q);
}

int main (void) { 
    int fda; 
    int fdc; 
    int totalMemory = 500; 
    int cMemory = totalMemory; 
    int frameSize = 50; 
    int totalFrames = 10; 
    int freeSpace[11]; 
    int i;
    int burst; 
    int counter = 0;
    int numOfClients;
    freeSpace[0] = 0;
    PCB currPCB; 
    PCB tempPCB;
  
    struct response {
        int memory;
        int error;
        int framesAssigned; 
        int frag; 
        int completion; 
    } output;

    Queue Q;
    Q.head = NULL;
    Q.tail = NULL;
    Q.sz = 0;

    for (i = 1; i < 11; i++) {
        freeSpace[i] = i; 
    }
    
    memset(tempPCB.privateFIFO,0,12);

    if ((mkfifo("FIFO_to_server",0666)<0 && errno != EEXIST)) {
        perror("cant create FIFO_to_server");
        exit(-1);
    }
   
    if ((fda=open("FIFO_to_server", O_RDONLY | O_NONBLOCK))<0)
        printf("cant open fifo to read");

	  printf("Enter the number of clients: ");
	  scanf("%d", &numOfClients);
    int done = numOfClients; 
	  printf("\nThis server currently has %d memory units and %d frames availabe for %d clients.\n", cMemory, totalFrames, numOfClients);
    printf("Frames Available:\n"); 
    
    for (i = 1; i < 11; i++) {
        if (freeSpace[i] == i)
            printf("[%d]",freeSpace[i]);
        }
   
    while (numOfClients != 0) {
	      int incoming = 0;
        
        do {
            incoming = read(fda, &tempPCB, sizeof(tempPCB)); 
        } while (incoming > 0 && incoming < sizeof(tempPCB));
            
        if (incoming == sizeof(tempPCB)) {
            if (tempPCB.memory < totalMemory && tempPCB.memory < cMemory) {
				        printf("\nClient%s arrived.\n", tempPCB.privateFIFO);
                    if (tempPCB.memory % frameSize != 0 && (tempPCB.memory / frameSize) < totalFrames) {
                        tempPCB.numOfFrames = (tempPCB.memory / frameSize) + 1; 
                        totalFrames = totalFrames - tempPCB.numOfFrames; 
                        tempPCB.frag = frameSize - (tempPCB.memory % frameSize); 
                    }
                
                    else if (tempPCB.memory % frameSize == 0 && (tempPCB.memory / frameSize) <= totalFrames)  {
                        tempPCB.numOfFrames = (tempPCB.memory / frameSize);
                         totalFrames = totalFrames - tempPCB.numOfFrames;
                     }
                     
            cMemory = cMemory - tempPCB.memory; 
            int check = 0;
            int framesNeeded = tempPCB.numOfFrames;
                
                while (framesNeeded != 0) {
                    if (freeSpace[check] != 0) {
                        tempPCB.frames[check] = check;
                        freeSpace[check] = 0;
                        framesNeeded--;
                    }
                    
					          else if (freeSpace[check] == 0) {
						            tempPCB.frames[check] = 0;
					          }
                    
			              check++; 
                }
  
				    output.memory = tempPCB.memory;
				    output.error = 3; // no error
				    enqueue(&Q,tempPCB); // Adds to queue
			    	numOfClients--;
            }
		
			  else if (tempPCB.memory > totalMemory) {
            output.memory = 0; 
            output.error = 1; 
        
        if ((fdc = open(tempPCB.privateFIFO, O_WRONLY)) < 0)
             printf("Private_FIFO Error");
             write(fdc, &output, sizeof(output)); // Writes the resulting error to the client.
             }
        
        else if (tempPCB.memory > cMemory) {
            output.memory = 0; 
            output.error = 2;
            if ((fdc = open(tempPCB.privateFIFO, O_WRONLY)) < 0)
                printf("Private_FIFO Error");
            write(fdc, &output, sizeof(output)); // Writes the resulting error to the client.
        }
    }
}

        printf("This server currently has %d memory units and %d frames availabe.\n", cMemory, totalFrames);
        printf("Frames Available:\n"); 
        
        for (i = 1; i < 11; i++) {
            if (freeSpace[i] != 0)
                printf("[%d]",freeSpace[i]);
        }
	
        while (done != 0) {
	          burst = 5;
            currPCB = dequeue(&Q);
		
		        while (burst > 0) {
                printf("\nClock time: %d\n", counter);
                sleep(1); 
                currPCB.CPUBurst = currPCB.CPUBurst - 1;
                burst--;
                counter++;
		         }

     if (currPCB.CPUBurst == 0) {
        if ((fdc = open(currPCB.privateFIFO, O_WRONLY)) < 0) 
            printf("Private_FIFO Error");
        
        output.completion = counter;
        printf("Job for %s completed at %d\n", currPCB.privateFIFO, counter);
          
        for (i = 0; i <= 11; i++) {
            if (currPCB.frames[i] == i)
                freeSpace[i] = i;
				    else if (freeSpace[i] == 0)
                freeSpace[i] = 0;
					  }
			
        totalFrames = totalFrames + currPCB.numOfFrames; 
	      cMemory = cMemory + currPCB.memory; 
	      printf("This server currently has %d memory units and %d frames availabe.\n", cMemory, totalFrames);
        printf("Frames Available:\n"); 
        
        for (i = 1; i < 11; i++) {
            if (freeSpace[i] != 0)
                printf("[%d]",freeSpace[i]);
        }
        
        output.framesAssigned = currPCB.numOfFrames; 
        output.frag = currPCB.frag; 
        write(fdc, &output, sizeof(output)); 
        done--;
			 }
       
     else 
		     enqueue(&Q, currPCB);
     }

    printf("\nServer is closing.");
    close(fda); 
    unlink("FIFO_to_server"); 
}

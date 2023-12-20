	#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>  
#define NUM_PRODUCERS 4 
#define NUM_CONSUMERS 2
#define MESSAGE_LENGTH 20
#define NUM_MESSAGES 2 // how many slots they are
#define SHM_SIZE (sizeof(struct Message) * (NUM_MESSAGES+1)) //can only hold two messages
struct Message {
    char content[MESSAGE_LENGTH + 1]; // +1 for null terminator
};
struct sharedvar{
int nb[2];
sem_t NBvide[2],NBplein[2],MUTEX[2],MUTEX2[2],MUTEXfile,sharr;

};



int countLines(FILE *fp);

struct Message getMessage(int randomness);


void producers(int shmid,int grp,int randomness,struct sharedvar *sharedvariab);
void consumers(int shmid,int grp,struct sharedvar *sharedvariab);

int main(){
int pid;
printf("not");
//srand

srand(time(NULL));
//init the semaphores
int i;






//shared memory creation
key_t key = ftok("contentshared.txt", 1);
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);

   


struct Message *shared_message = (struct Message *)shmat(shmid, NULL, 0);


struct sharedvar *sharedvariab = (struct sharedvar *)(shared_message + NUM_MESSAGES);

	sharedvariab->nb[0]=0;
	sharedvariab->nb[1]=0;
sem_init(&sharedvariab->MUTEXfile,0,1);
sem_init(&sharedvariab->sharr,0,1);
sem_init(&sharedvariab->MUTEX[0],0,1);
sem_init(&sharedvariab->MUTEX[1],0,1);
sem_init(&sharedvariab->MUTEX2[0],0,1);
sem_init(&sharedvariab->MUTEX2[1],0,1);
sem_init(&sharedvariab->NBvide[0],0,NUM_MESSAGES);
sem_init(&sharedvariab->MUTEXfile,0,NUM_MESSAGES);
sem_init(&sharedvariab->NBplein[0],0,0);
sem_init(&sharedvariab->NBplein[1],0,0);


if (sem_init(&sharedvariab->sharr, 1, 1) == -1 ||
    sem_init(&sharedvariab->MUTEXfile, 1, 1) == -1 ||
    sem_init(&sharedvariab->MUTEX[0], 1, 1) == -1 ||
    sem_init(&sharedvariab->MUTEX[1], 1, 1) == -1 ||
    sem_init(&sharedvariab->MUTEX2[0], 1, 1) == -1 ||
    sem_init(&sharedvariab->MUTEX2[1], 1, 1) == -1 ||
    sem_init(&sharedvariab->NBplein[0], 1, 0) == -1 ||
    sem_init(&sharedvariab->NBplein[1], 1, 0) == -1) {
    perror("Semaphore initialization failed");
    exit(EXIT_FAILURE);
}

    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

printf("\n no here \n");

// Start consumer processes
  for(int i = 0; i < NUM_CONSUMERS; i++) {
    pid = fork();
    if(pid == 0) {
printf("consumer create grp %d num %d \n",i+1,i+1);
      consumers(shmid,i+1,sharedvariab);
      exit(0);  
    }
  }


  // Start producer processes 
  for(int i = 0; i < NUM_PRODUCERS; i++) {
    pid = fork();
    if(pid == 0) {
if(i>1){
printf("producer created grp %d num %d \n",2,i%4+1);
 producers(shmid,2,i%4+1,sharedvariab);
}
else{
printf("producer created grp %d num %d \n",1,i%4+1);
 producers(shmid,1,i%4+1,sharedvariab);
}

     
      exit(0);
   }
  }

  // Parent waits for all processes
  while(wait(NULL) > 0);
  
  return 0;

sem_destroy(&sharedvariab->MUTEXfile);
sem_destroy(&sharedvariab->MUTEX[0]);
sem_destroy(&sharedvariab->MUTEX[1]);
sem_destroy(&sharedvariab->MUTEX2[0]);
sem_destroy(&sharedvariab->MUTEX2[1]);
sem_destroy(&sharedvariab->NBvide[0]);
sem_destroy(&sharedvariab->NBvide[1]);
sem_destroy(&sharedvariab->NBplein[0]);
sem_destroy(&sharedvariab->NBplein[1]);




shmdt(shared_message);
shmdt(sharedvariab);
shmctl(shmid, IPC_RMID, NULL);


return 0;
}


int countLines(FILE *fp) {
    int lines = 0;
    int ch;
	if(fp !=NULL){
lines++;
}
else{
return 0;
}
    while ((ch = fgetc(fp)) != EOF) {

        if (ch == '\n' ) {
            lines++;

        }

	
    }

    rewind(fp);

    return lines;
}

struct Message getMessage(int randomness) {
FILE *fp = fopen("contentshared.txt", "r");

    struct Message message;

if(fp==NULL){
printf("error");
return message;
}
    
int tmp=0;
    int i = 1;
    char buffer[MESSAGE_LENGTH];
     int ch;
    int min = 1;
    int max = countLines(fp)-1;
    int randline = (rand() % max) + min;
   if(randline >randomness){
randline=randline-randomness;
}
else{
randline = (rand() % max) + min;
}
    /*
    printf("Total lines: %d\n", max);
    printf("Random line: %d\n", randline);
*/
    rewind(fp);
	
while ((ch = fgetc(fp)) != EOF) {

        if (ch == '\n' ) {
if (i == randline) {
                // Copy the content of buffer to message.content
                strncpy(message.content, buffer, sizeof(message.content) - 1);
                message.content[sizeof(message.content) - 1] = '\0'; // Ensure null termination
               
                break;
            }

           i++;
	memset(buffer, 0, sizeof(buffer));
tmp=0;

        }

	else{
if (tmp < sizeof(buffer) - 1) {
                buffer[tmp] = (char)ch;
tmp++;


            }


}
    }
	
	fclose(fp);
    return message;
    
}


void producers(int shmid,int grp,int randomness,struct sharedvar *sharedvariab){



while(1){
struct Message contentmessage;
//produire



//taking the shared array if it's the first of the group 
	
if (sem_wait(&sharedvariab->MUTEXfile) == -1) {
    perror("sem_wait MUTEXfile");
    exit(EXIT_FAILURE);
}

// ... rest of the code ...

contentmessage = getMessage(randomness);
printf("\n  proc :%d %d : %s \n", grp, randomness, contentmessage.content);


if (sem_post(&sharedvariab->MUTEXfile) == -1) {
    perror("sem_post MUTEXfile");
    exit(EXIT_FAILURE);
}



if(grp==1 || grp==2){

if (sem_wait(&sharedvariab->MUTEX[grp-1]) == -1) {
    perror("sem_wait MUTEX \n");
    exit(EXIT_FAILURE);
}
printf("lock: %d mutex  \n",randomness);
sharedvariab->nb[grp-1]++;
printf("nb %d %d \n",grp,sharedvariab->nb[grp-1]);

if(sharedvariab->nb[grp-1]==1){
if (sem_wait(&sharedvariab->sharr) == -1) {
    perror("sem_wait sharr");
    exit(EXIT_FAILURE);
}

}
if (sem_post(&sharedvariab->MUTEX[grp-1]) == -1) {
    perror("sem_post MUTEX");
    exit(EXIT_FAILURE);
}

printf("unlock: %d mutex  \n",randomness);
}







//S.C
if(grp==1 || grp==2){
if (sem_wait(&sharedvariab->NBvide[grp-1]) == -1) {
    perror("sem_wait NBvide");
    exit(EXIT_FAILURE);
}
if (sem_wait(&sharedvariab->MUTEX2[grp-1]) == -1) {
    perror("sem_wait MUTEX2");
    exit(EXIT_FAILURE);
}

struct Message *shared_message = (struct Message *)shmat(shmid, NULL, 0);
int emptyslot = -1;

for (int i = 0; i < NUM_MESSAGES; i++) {
printf("if  place : %s \n",shared_message[i].content);
    if (shared_message[i].content[0] == '\0') {
        emptyslot = i;
        break;
    }
}

if (emptyslot != -1) {
    
    strcpy(shared_message[emptyslot].content, contentmessage.content);
    } else {
   
    printf("no empty slots available for producer %d.\n", randomness);
}  

shmdt(shared_message);
if (sem_post(&sharedvariab->MUTEX2[grp-1]) == -1) {
    perror("sem_post MUTEX2");
    exit(EXIT_FAILURE);
}




printf("  %d: beforerealse \n", randomness);

    // Release NBplein semaphore
if (sem_post(&sharedvariab->NBplein[grp-1]) == -1) {
    perror("sem_post NBplein");
    exit(EXIT_FAILURE);
}
    
    printf("  %d: Released \n", randomness);


}



sleep(1);

}
}


void consumers(int shmid, int grp,struct sharedvar *sharedvariab) {





while(1){

   printf("Consumer grp %d: Before\n", grp);
if (sem_wait(&sharedvariab->NBplein[grp-1]) == -1) {
    perror("sem_wait NBplein");
    exit(EXIT_FAILURE);
}


 
    printf("Consumer grp %d: After \n", grp);

    struct Message *shared_message = (struct Message *)shmat(shmid, NULL, 0);

    // Add debug prints to see the current state of shared_message
    for (int i = 0; i < NUM_MESSAGES; i++) {
        printf("Consumer grp %d: shared_message[%d].content = %s\n", grp, i, shared_message[i].content);
    }

    struct Message contentmessage;
    strncpy(contentmessage.content, shared_message[0].content, sizeof(contentmessage.content) - 1);
    contentmessage.content[sizeof(contentmessage.content) - 1] = '\0';
    

    for (int i = 0; i < NUM_MESSAGES - 1; i++) {

        strncpy(shared_message[i].content, shared_message[i + 1].content, sizeof(shared_message[i].content) - 1);
        shared_message[i].content[sizeof(shared_message[i].content) - 1] = '\0';
    }
    
    memset(shared_message[NUM_MESSAGES - 1].content, 0, sizeof(shared_message[NUM_MESSAGES - 1].content));
for (int i = 0; i < NUM_MESSAGES; i++) {
        printf("testing :Consumer grp %d: shared_message[%d].content = %s\n", grp, i, shared_message[i].content);
    }

    shmdt(shared_message);

if (sem_post(&sharedvariab->NBvide[grp-1]) == -1) {
    perror("sem_post NBvide");
    exit(EXIT_FAILURE);
}
  

    if (grp == 1 || grp == 2) {
if (sem_wait(&sharedvariab->MUTEX[grp-1]) == -1) {
    perror("sem_wait MUTEX");
    exit(EXIT_FAILURE);
}

  
	sharedvariab->nb[grp-1]--;
      
printf("recopying in grp %d %d \n",grp,sharedvariab->nb[grp-1]);
        if (sharedvariab->nb[grp-1] == 0) {
            printf("finally grp :%d \n",grp);
if (sem_post(&sharedvariab->sharr) == -1) {
    perror("sem_post sharr");
    exit(EXIT_FAILURE);
}
 
        }
else if(sharedvariab->nb[grp-1]==-1){
printf("stopping \n");
exit(0);

}

if (sem_post(&sharedvariab->MUTEX[grp-1]) == -1) {
    perror("sem_post MUTEX");
    exit(EXIT_FAILURE);
}

    }
sleep(1);
}


}



#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
  int fd, i,nloop=10,zero=0,*counter_ptr,*BankAccount;
  sem_t *mutex;
  int time = rand() % 5;

  //open a file and map it into memory this is to hold the shared counter

  fd = open("log.txt",O_RDWR|O_CREAT,S_IRWXU);
  write(fd,&zero,sizeof(int));
  BankAccount = mmap(NULL,sizeof(int),PROT_READ |PROT_WRITE,MAP_SHARED,fd,0);
  close(fd);

  /* create, initialize semaphore */
 if ((mutex = sem_open("examplesemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
    perror("semaphore initilization");
    exit(1);
  }
 
  if (fork() == 0) { /* child process*/
    for (i = 0; i < nloop; i++) {
      sleep(time);
      printf("Poor Student: Attempting to Check Balance\n");
      sem_wait(mutex);
      int randomNum = rand();
      int balance = *BankAccount;
      if (randomNum % 2 != 0) {
        printf("Poor Student: Last Checking Balance = $%d\n", balance);
      }
      else {
        int amountNeeded = rand() % 50;
        printf("Poor Student needs $%d\n", amountNeeded);
        
        if (amountNeeded <= balance) {
          balance =- amountNeeded;
          printf("Poor Student: Withdraws $%d / Balance = $%d\n", amountNeeded, balance);
        }
        else {
          printf("Poor Student: Not Enough Cash ($%d)\n", balance);
        }
        *BankAccount = balance;
      }
      sem_post(mutex);
      sleep(1);
    }
    exit(0);
  }
  /* back to parent process */
  for (i = 0; i < nloop; i++) {
    sleep(time);
    printf("Dear Old Dad: Attempting to Check Balance\n");
    sem_wait(mutex);
    int randomNum = rand();
    int balance = *BankAccount;
    if (randomNum % 2 != 0) {
      printf("Dear Old Dad: Last Checking Balance = $%d\n", balance);
    }
    else {
      if (balance < 100){
        int amount = rand() % 100;
        if (amount % 2 == 0){
          balance += amount;
          printf("Dear Old Dad: Last Checking Balance = $%d\n", balance);
          *BankAccount = balance;
        }
        else {
          printf("Dear old Dad: Doesn't have any money to give\n");
        }
        printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", balance);
      }
      sem_post(mutex);
      sleep(1);
    }
  }
  exit(0);
}

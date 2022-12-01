//написать программу получающую в качестве параметров два номера. Первый номер это 
//номер для получения сообщений, второй номер для передачи сообщений. Можно  
//запустить несколько экземпляров такой программы и они все будут обмениваться  
//сообщениями между собой по номерам.
// Всё это нужно реализовать через одну очередь. 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define MESSAGE_LENGTH 255

typedef struct mymsgbuf_ {
  long mtype;
  long sender_id;
  char mtext[MESSAGE_LENGTH];
} mymsgbuf;

void sender_thread(key_t key, long sender_id, long receiver_id);
void receiver_thread(key_t key, long receiver_id);

int main(int argc, char* argv[]) //first - receiver_id, second - sender_id
{
  pid_t chpid = 0;
  int   msqid = 0;
  char  pathname[] = "queue_2.c";
  key_t key   = 0;
  int   len   = 0, maxlen = 0;
  
  if (argc != 3) {
	  printf("A few params\n");
    exit(EXIT_FAILURE);
  }

  long sender_id = 0, receiver_id = 0;
  receiver_id = strtol(argv[1], NULL, 10);
  sender_id   = strtol(argv[2], NULL, 10);

  key = ftok(pathname, 0);
  if (key == -1) {
	  printf("something wrong in keygen\n");
  }

  // Если очередь уже существует, то ничего не делаем  
  if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
  {
    printf("Something wrong in msgget\n");
  }
    
  chpid = fork();
  //один из процессов проверяет очередь на предмет наличия в ней сообщений,
  // адресованных данному терминалу и выводит их на экран
 
  // второй процесс ожидает ввода с клавиатуры и отправляет сообщения,
  // встретив символ перевода строки 
  if (chpid < 0){
	  printf("Error in children");
  }
  else if (chpid == 0) {
	  receiver_thread(key, receiver_id);
  }
  else {
	  sender_thread(key, sender_id, receiver_id);
  }
  return 0;  
}


void sender_thread(key_t key, long sender_id, long receiver_id)
{
  int msqid = 0;
  int len   = 0, maxlen = 0;

  if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get msqid, sender\n");
  }

  mymsgbuf mybuf = {};
  while(1) {
	  char* line = NULL;
	  size_t len_line = 0;

  	getline(&line, &len_line, stdin);
    mybuf.mtype = sender_id;
    mybuf.sender_id = receiver_id;
    strcpy(mybuf.mtext, line);
	  len = strlen(mybuf.mtext) + 1 + sizeof(long);

	  if (msgsnd(msqid, (struct msgbuf*) &mybuf, len, 0) < 0) {
		  printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
      exit(EXIT_FAILURE);
    }
	  free(line);
  } 
}


void receiver_thread(key_t key, long receiver_id)
{
  int msqid = 0;
  int len   = 0, maxlen = 0;
  mymsgbuf mybuf = {};

  if ((msqid = msgget(key, 0666)) < 0) {
    printf("Can\'t get msqid\n");
    exit(-1);
  }

  while (1) {
    maxlen = MESSAGE_LENGTH + sizeof(long);
    
    if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, receiver_id, 0)) < 0)
    {
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }
    
    printf("User %ld: %s", mybuf.sender_id, mybuf.mtext);
  }
}


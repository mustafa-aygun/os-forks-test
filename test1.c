#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int waitTillDay5 = 1;
time_t signalTime;

struct Output
{

  double execTime;
  int pidID;
  char signal[50];
  int m;
  int *numbers;
};

int *selectionSort(int[], int);
int *insertionSort(int[], int);
void createFile(int, int);
void signalHandlerUSR1(int);
void signalHandlerUSR2(int);
void childOperations(int);
void childOperationsForPipe(int, struct Output *);
void parentOperation(int, struct Output **);
void parentOperationForPipe(int, struct Output **);
void forks(int);
void forksAnPipe(int);

int main(int argc, char const *argv[])
{

  int n, m, i;
  printf("Welcome to app\n");
  printf("Enter number of file: ");
  scanf("%d", &n);
  printf("Enter size of numbers: ");
  scanf("%d", &m);
  createFile(m, n);
  forksAnPipe(n);
  printf("Back\n");
  return 0;
}

void forks(int n)
{
  int i;
  struct Output **myResults = malloc(n * sizeof(struct Output *));
  for (i = 0; i < n; i++)
  {
    myResults[i] = malloc(sizeof(struct Output));
  }
  pid_t pid[n];
  for (i = 0; i < n; i++)
  {
    signal(SIGUSR2, signalHandlerUSR2);
    signal(SIGUSR1, signalHandlerUSR1);
    ;

    if ((pid[i] = fork()) == 0)
    {
      srand(time(0) - i);
      while (waitTillDay5)
        sleep(1);
      time(&signalTime);
      childOperations(i);
      _exit(0);
    }
  }
  for (i = 0; i < n; i++)
  {
    if (pid[i] % 2 == 0)
    {
      kill(pid[i], SIGUSR2);
    }
    else
    {
      kill(pid[i], SIGUSR1);
    }
  }

  for (int i = 0; i < n; i++)
  {
    wait(NULL);
  }

  for (int k = 0; k < n; k++)
  {
    myResults[k]->pidID = pid[k];
    printf("ID: %d\n", myResults[k]->pidID);
  }
  parentOperation(n, myResults);
}

void forksAnPipe(int n)
{
  int i, child_status;
  struct Output **myResults = (struct Output **)malloc(n * sizeof(struct Output *));
  for (i = 0; i < n; i++)
  {
    myResults[i] = malloc(sizeof(struct Output));
  }
  int fd[n][2];
  for (i = 0; i < n; i++)
  {
    if (pipe(fd[i]) == -1)
    {
      printf("error with the pipe 1. \n");
      return;
    }
  }
  pid_t pid[n];
  for (i = 0; i < n; i++)
  {
    signal(SIGUSR2, signalHandlerUSR2);
    signal(SIGUSR1, signalHandlerUSR1);

    if ((pid[i] = fork()) == 0)
    {
      srand(time(0) - i);
      struct Output *myData = malloc(sizeof(struct Output));
      close(fd[i][0]);
      while (waitTillDay5)
        sleep(1);
      time(&signalTime);
      childOperationsForPipe(i, myData);
      if (write(fd[i][1], &myData->m, sizeof(int)) == -1)
      { // note variable reference here
        printf("error with the pipe write 1. \n");
        return;
      }

      if (write(fd[i][1], myData->numbers, myData->m * sizeof(int)) == -1)
      { // note array reference and size of array here
        printf("error with the pipe write 2. \n");
        return;
      }
      if (write(fd[i][1], myData, sizeof(struct Output)) == -1)
      { // note variable reference here
        printf("error with the pipe write 3. \n");
        return;
      }
      close(fd[i][1]);
      _exit(0);
    }
  }

  for (i = 0; i < n; i++)
  {
    if (pid[i] % 2 == 0)
    {
      kill(pid[i], SIGUSR2);
    }
    else
    {
      kill(pid[i], SIGUSR1);
    }
  }

  for (i = 0; i < n; i++)
  {
    close(fd[i][1]);
    struct Output *myDataFromPipe = malloc(sizeof(struct Output));

    int myMFromPipe;
    if (read(fd[i][0], &myMFromPipe, sizeof(int)) < 0) // note variable reference here
    {
      printf("error with the pipe read 1. \n");
      return;
    }
    int *myNumbersFromPipe = malloc(myMFromPipe * sizeof(int)); // dynamically allocate array

    if (read(fd[i][0], myNumbersFromPipe, myMFromPipe * sizeof(int)) < 0) // note array reference and size here
    {
      printf("error with the pipe read 2. \n");
      return;
    }

    if (read(fd[i][0], myDataFromPipe, sizeof(struct Output)) < 0) // pay attention to struct address reference here
    {
      printf("error with the pipe read 3. \n");
      return;
    }
    myResults[i] = myDataFromPipe;
    myResults[i]->numbers = myNumbersFromPipe;
    close(fd[i][0]);
  }
  for (int i = 0; i < n; i++)
  {
    wait(&child_status);
  }
  for (int k = 0; k < n; k++)
  {
    myResults[k]->pidID = pid[k];
    printf("ID: %d, Exec: %f M: %d\n", myResults[k]->pidID, myResults[k]->execTime, myResults[k]->m);
    for (int z = 0; z < myResults[k]->m; z++)
    {
      printf("%d- ", myResults[k]->numbers[z]);
    }
  }
  parentOperationForPipe(n, myResults);
}

void createFile(int m, int n)
{
  FILE *myFile;
  char filename[20];
  int i, j;
  for (i = 0; i < n; i++)
  {

    sprintf(filename, "input%d.txt", i);
    myFile = fopen(filename, "w");
    fprintf(myFile, "%d\n", m);
    for (j = 0; j < m; j++)
    {
      fprintf(myFile, "%d ", rand() % m);
    }
    fclose(myFile);
  }
}

int swap(int *x, int *y)
{
  int temp = *x;
  *x = *y;
  *y = temp;
}

int *selectionSort(int *mylist, int size)
{

  int i, j, min;
  for (i = 0; i < size - 1; i++)
  {
    min = i;
    for (j = i + 1; j < size; j++)
    {
      if (mylist[j] < mylist[min])
      {
        min = j;
      }
    }
    swap(&mylist[min], &mylist[i]);
  }

  return mylist;
}

int *insertionSort(int *myList, int size)
{
  int i, j, value;
  for (i = 0; i < size; i++)
  {
    value = myList[i];
    for (j = i - 1; - 1 < j; j--)
    {
      if (myList[j] > value)
      {
        myList[j + 1] = myList[j];
      }
      else
      {
        break;
      }
    }
    myList[j + 1] = value;
  }
  return myList;
}

void signalHandlerUSR1(int signumber)
{
  waitTillDay5 = 0;
}

void signalHandlerUSR2(int signumber)
{
  waitTillDay5 = 0;
}

void childOperations(int fileNum)
{
  clock_t startTime, endTime;
  int i = 0, j;
  FILE *myFile;
  char filename[20], filename2[20];
  sprintf(filename, "input%d.txt", fileNum);

  myFile = fopen(filename, "r");

  struct Output *myData = malloc(sizeof(struct Output));

  if (myFile != NULL)
  {
    if (fscanf(myFile, "%d\n", &myData->m) != EOF)
      myData->numbers = (int *)malloc(myData->m * sizeof(int));
    for (j = 0; j < myData->m; j++)
    {
      if (fscanf(myFile, "%d ", &myData->numbers[j]) != EOF)
        ;
    }
  }
  fclose(myFile);
  int pidId = getpid();
  startTime = clock();
  if (pidId % 2 == 0)
  {
    myData->numbers = selectionSort(myData->numbers, myData->m);
  }
  else
  {
    myData->numbers = insertionSort(myData->numbers, myData->m);
  }
  endTime = clock();
  int randomSleepTime = rand() % 7 + 1;
  sleep(randomSleepTime);
  double execTime = (double)(endTime - startTime) / CLOCKS_PER_SEC + randomSleepTime;
  sprintf(filename2, "output%d.txt", fileNum);

  myFile = fopen(filename2, "w");
  fprintf(myFile, "%d\n", myData->m);
  for (j = 0; j < myData->m; j++)
  {
    fprintf(myFile, "%d ", myData->numbers[j]);
  }
  fprintf(myFile, "\n%lf\n", execTime);
  if (pidId % 2 == 0)
  {
    fprintf(myFile, "SGUSR2 %s", ctime(&signalTime));
  }
  else
  {
    fprintf(myFile, "SGUSR1 %s", ctime(&signalTime));
  }

  fclose(myFile);
}

void childOperationsForPipe(int fileNum, struct Output *myOut)
{
  clock_t startTime, endTime;
  int i = 0, j;
  FILE *myFile;
  char filename[20];
  sprintf(filename, "input%d.txt", fileNum);

  myFile = fopen(filename, "r");

  if (myFile != NULL)
  {
    if (fscanf(myFile, "%d\n", &myOut->m) != EOF)
    {
      myOut->numbers = (int *)malloc(myOut->m * sizeof(int));
      for (j = 0; j < myOut->m; j++)
      {
        if (fscanf(myFile, "%d ", &myOut->numbers[j]) != EOF)
          ;
      }
    }
  }
  fclose(myFile);
  printf("%d Here", myOut->m);
  startTime = clock();
  int pidId = getpid();
  if (pidId % 2 == 0)
  {
    myOut->numbers = selectionSort(myOut->numbers, myOut->m);
  }
  else
  {
    myOut->numbers = insertionSort(myOut->numbers, myOut->m);
  }
  endTime = clock();
  int randomSleepTime = rand() % 7 + 1;
  sleep(randomSleepTime);
  double execTime = (double)(endTime - startTime) / CLOCKS_PER_SEC + randomSleepTime;

  myOut->execTime = execTime;
}

void parentOperation(int n, struct Output **myResults)
{

  int i, j;
  double value;
  FILE *myFile;
  for (i = 0; i < n; i++)
  {
    char filename[20];
    sprintf(filename, "output%d.txt", i);
    myFile = fopen(filename, "r");
    if (fscanf(myFile, "%d\n", &myResults[i]->m) != EOF)
      ;
    myResults[i]->numbers = (int *)malloc(myResults[i]->m * sizeof(int));
    for (j = 0; j < myResults[i]->m; j++)
    {
      if (fscanf(myFile, "%d ", &myResults[i]->numbers[j]) != EOF)
        ;
    }
    if (fscanf(myFile, "\n%lf\n", &myResults[i]->execTime) != EOF)
      ;
    while(fgets(myResults[i]->signal,50,myFile));
    fclose(myFile);
  }
  int min;
  struct Output *myTemp;
  for (i = 0; i < n - 1; i++)
  {
    min = i;
    for (j = i + 1; j < n; j++)
    {
      if (myResults[j]->execTime < myResults[min]->execTime)
      {
        min = j;
      }
    }
    myTemp = myResults[i];
    myResults[i] = myResults[min];
    myResults[min] = myTemp;
  }

  myFile = fopen("output.txt", "w");
  for (i = 0; i < n; i++)
  {
    fprintf(myFile, "%d %lf ", myResults[i]->pidID, myResults[i]->execTime);
    for (j = 0; j < myResults[i]->m; j++)
    {
      fprintf(myFile, "%d ", myResults[i]->numbers[j]);
    }
    fprintf(myFile, "%s\n",myResults[i]->signal);
  }
  fclose(myFile);
}

void parentOperationForPipe(int n, struct Output **myResults)
{

  int i, j;
  double value;
  int min;
  struct Output *myTemp;
  for (int k = 0; k < n; k++)
  {
    printf("ID: %d, Exec: %f\n", myResults[k]->pidID, myResults[k]->execTime);
    for (int z = 0; z < myResults[k]->m; z++)
    {
      printf("%d ", myResults[k]->numbers[z]);
    }
  }
  char *ctime(const time_t *signalTime);
  for (i = 0; i < n - 1; i++)
  {
    min = i;
    for (j = i + 1; j < n; j++)
    {
      if (myResults[j]->execTime < myResults[min]->execTime)
      {
        min = j;
      }
    }
    myTemp = myResults[i];
    myResults[i] = myResults[min];
    myResults[min] = myTemp;
    strcpy(myResults[i]->signal,ctime);
  }
  
  
  FILE *myFile;
  myFile = fopen("output.txt", "w");
  for (i = 0; i < n; i++)
  {
    fprintf(myFile, "%d %lf ", myResults[i]->pidID, myResults[i]->execTime);
    for (j = 0; j < myResults[i]->m; j++)
    {
      fprintf(myFile, "%d ", myResults[i]->numbers[j]);
    } 
    fprintf(myFile,"%s\n",myResults[i]->signal);
  }
  fclose(myFile);
}
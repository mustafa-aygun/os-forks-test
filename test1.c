#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>



int waitTillDay5 = 1;
time_t signalTime;

struct Data
{
    int number;
    int *array;
};

struct Output{
  
  double execTime;
  int pidID;
  char signal[6][50];
  struct Data *sortedData;  
};

int *selectionSort(int[], int);
int *insertionSort(int[], int);
void createFile(int, int);
void signalHandlerUSR1(int);
void signalHandlerUSR2(int);
void childOperations(int);
void parentOperation(int,struct Output**);
void forks(int);

int main(int argc, char const *argv[])
{
    srand(time(0));
    int n, m, i;
    printf("Welcome to app\n");
    printf("Enter number of file: ");
    scanf("%d", &n);
    printf("Enter size of numbers: ");
    scanf("%d", &m);

    createFile(m, n);

    forks(n);

    printf("Back\n");
    return 0;
}

void forks(int n)
{   
    int i;
    struct Output **myResults = (struct Ouput*)malloc(n*sizeof(struct Output*));
    for(i = 0; i < n; i++){
      myResults[i] = malloc(sizeof(struct Output));
      myResults[i]->sortedData = malloc(sizeof(struct Data));
    }
    pid_t pid[n];
    for (i = 0; i < n; i++)
    {
        signal(SIGUSR2, signalHandlerUSR2);
        signal(SIGUSR1, signalHandlerUSR1);
        ;
        
        if ((pid[i] = fork()) == 0)
        {
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
    
    for(int k = 0; k < n; k++){
      myResults[k]->pidID = pid[k];
      printf("ID: %d\n",myResults[k]->pidID);
    }
    parentOperation(n,myResults);
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
    for (i = 0; i < size; i++){
      value = myList[i];
      for (j = i - 1; - 1 < j; j--){
        if (myList[j] > value){
          myList[j + 1] = myList[j];
        }
        else{
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
  clock_t startTime, endTime1,endTime2;
  int i = 0, j;
  FILE *myFile;
  char filename[20], filename2[20];
  sprintf(filename, "input%d.txt", fileNum);

  myFile = fopen(filename, "r");

  struct Data *myData = malloc(sizeof(struct Data));

  if (myFile != NULL)
  {
    if (fscanf(myFile, "%d\n", &myData->number) != EOF)
    myData->array = (int *)malloc(myData->number * sizeof(int));
    for (j = 0; j < myData->number; j++){
      if (fscanf(myFile, "%d ", &myData->array[j]) != EOF);
    }
  }
  fclose(myFile);

  startTime = clock();
  int pidId = getpid();
  if (pidId % 2 == 0){
    myData->array = selectionSort(myData->array, myData->number);
  }
  else{
    myData->array = insertionSort(myData->array, myData->number);
  }
  endTime1 = clock(); 
  int randomSleepTime = rand()%7+1;
  sleep(randomSleepTime);
  endTime2 = clock(); 
  double execTime2 = endTime1-startTime;
  double execTime = endTime2-startTime;
  sprintf(filename2, "output%d.txt", fileNum);

  myFile = fopen(filename2, "w");
  fprintf(myFile, "%d\n", myData->number);
  for (j = 0; j < myData->number; j++){
    fprintf(myFile, "%d ", myData->array[j]);
  }
  fprintf(myFile,"\n%lf\n",execTime);  
  /*fprintf(myFile,"%d",randomSleepTime);  
  fprintf(myFile,"\n%lf\n",execTime2);  */
  if (pidId % 2 == 0){
    fprintf(myFile,"SGUSR2 %s",ctime(&signalTime));
  }
  else{
    fprintf(myFile,"SGUSR1 %s",ctime(&signalTime));
  }
    

  fclose(myFile);
}

void parentOperation(int n,struct Output** myResults){

  int i,j;
  double value;
  FILE *myFile;
  for(i = 0; i < n; i++){    
    char filename[20];
    sprintf(filename, "output%d.txt", i);
    myFile = fopen(filename, "r");
    if (fscanf(myFile, "%d\n", &myResults[i]->sortedData->number) != EOF);
    myResults[i]->sortedData->array = (int *)malloc(myResults[i]->sortedData->number*sizeof(int));
    for(j = 0; j < myResults[i]->sortedData->number; j++){
      if (fscanf(myFile, "%d ", &myResults[i]->sortedData->array[j]) != EOF);
    }
    if(fscanf(myFile,"\n%lf\n",&myResults[i]->execTime) != EOF);
    for(j = 0; j < 6; j++){
      if(fscanf(myFile,"%s ",myResults[i]->signal[j]) != EOF);
    }
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

  myFile = fopen("output.txt","w");
  for(i = 0; i < n; i++){
    fprintf(myFile,"%d %lf ",myResults[i]->pidID,myResults[i]->execTime);
    for(j = 0; j < myResults[i]->sortedData->number; j++){
      fprintf(myFile, "%d ", myResults[i]->sortedData->array[j]);
    }
    for(j = 0; j < 6; j++){
      fprintf(myFile,"%s ",myResults[i]->signal[j]);
    }
    fprintf(myFile,"\n");
  }
  fclose(myFile);


}

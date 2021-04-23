#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

int *selectionSort(int[], int);
int *insertionSort(int[], int);
void createFile(int, int);
void signalHandlerUSR1(int);
void signalHandlerUSR2(int);
void childOperations(int);
void forks(int);

int waitTillDay5 = 0;

struct Data
{
  int number;
  int *array;
};

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
  pid_t pid[n];
  for (i = 0; i < n; i++)
  {
    if ((pid[i] = fork()) == 0)
    {
      printf("I am here\n");
      signal(SIGUSR2, signalHandlerUSR2);
      signal(SIGUSR1, signalHandlerUSR1);
      while (waitTillDay5)
        sleep(1);
      childOperations(i);
      exit(1);
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
  waitTillDay5 = 1;
}

void signalHandlerUSR2(int signumber)
{
  waitTillDay5 = 1;
}

void childOperations(int fileNum)
{
  printf("hadi be brom3");
  int i = 0, j;
  FILE *myFile;
  char filename[20], filename2[20];
  sprintf(filename, "input%d.txt", fileNum);

  myFile = fopen(filename, "r");

  struct Data *myData = malloc(sizeof(struct Data));

  if (myFile != NULL)
  {
    while (i < 2)
    {
      if (i == 0)
      {
        if (fscanf(myFile, "%d\n", &myData->number) != EOF);
        i++;
      }
      else
      {
        myData->array = (int *)malloc(myData->number * sizeof(int));
        for (j = 0; j < myData->number; j++)
        {
          if (fscanf(myFile, "%d ", &myData->array[j]) != EOF);
          i++;
        }
      }
    }
  }
  fclose(myFile);

  myData->array = selectionSort(myData->array, myData->number);
  sprintf(filename2, "output%d.txt", fileNum);

  myFile = fopen(filename2, "w");
  fprintf(myFile, "%d\n", myData->number);
  for (j = 0; j < myData->number; j++)
  {
    fprintf(myFile, "%d ", myData->array[j]);
  }
}
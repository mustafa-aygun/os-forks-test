/* Mustafa Aygün 2315125 */
/*Libraries added*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int waitTillDay5 = 1; /*A global variable to control signal in each child.*/
time_t signalTime; /*A time variable to take signal time in each child.*/

/*Struct to both read it from file and also sending in pipes.*/
struct Output{
  double execTime; /*Double execTime to keep execeution time of sorting.*/
  int pidID; /*Integer pidID to keep child's ID.*/
  char signal[50]; /*It is character array to keep signal name and time.*/
  int m; /*Integer m to keep m which is length of numbers.*/
  int *numbers; /*Integer array numbers to keep numbers in it.*/
};
/*Function prototypes.*/
int *selectionSort(int[], int); /*Selection sort*/
int *insertionSort(int[], int); /*Insertion sort*/
void createFile(int, int); /*Creating n number of input files with length of m numbers.*/
void signalHandlerUSR1(int); /*USR1 signal handler*/
void signalHandlerUSR2(int); /*USR2 signal handler*/
void childOperations(int); /*Doing child operations for communicating with files.*/
void childOperationsForPipe(int, struct Output *);/*Doing child operations for communicating with pipes.*/
void parentOperation(int, struct Output **); /*Doing parent operations for communicating with files.*/
void parentOperationForPipe(int, struct Output **); /*Doing parent operations for communicating with pipes.*/
void forks(int); /*Doing forks for files.*/
void forksAnPipe(int); /*Doing forks for pipes.*/

int main(int argc, char const *argv[]){

  int n, m,menuOption=0; /*Definining variables and getting input of them at following.*/
  printf("Welcome to app\n");
  printf("Enter number of file: ");
  scanf("%d", &n);
  printf("Enter size of numbers: ");
  scanf("%d", &m);
  createFile(m, n); /*Creating n number of files with length of m numbers.*/
  printf("The Option Menu!\n1-Communicating with files\n2-Communicating with pipes\n3-Exit\nSelect option: ");
  while(menuOption != 3){ /*Selection menu for files or pipes.*/
    scanf("%d",&menuOption);
    if(menuOption == 1){
      forks(n); /*Call normal forks if selection is for files.*/
      break;
    }
    else if(menuOption == 2){
      forksAnPipe(n); /*Call forks with pipes if selection is for pipes.*/
      break;
    }
    else{
      printf("Please enter a valid option: "); /*Asking for valid option.*/
    }
  }
  return 0;
}

void forks(int n){
  int i; /*Integer i for running loops.*/
  struct Output **myResults = malloc(n * sizeof(struct Output *)); /*Malloc an array of struct.*/
  for (i = 0; i < n; i++){
    myResults[i] = malloc(sizeof(struct Output)); /*Malloc each node in the struct.*/
  }
  pid_t pid[n]; /*Creating n number of child.*/
  for (i = 0; i < n; i++){
    signal(SIGUSR2, signalHandlerUSR2); /*Calling signal handlers.*/
    signal(SIGUSR1, signalHandlerUSR1);
    if ((pid[i] = fork()) == 0){ /*Doing fork and child operations.*/
      srand(time(0) - i); /*Arranging srand to create random number for sleeping after sorting.*/
      while (waitTillDay5) /*Infinite while loop till signal is coming.*/
        sleep(1);
      time(&signalTime); /*Get signal coming time.*/
      childOperations(i); /*Call childOperations to do file operations.*/
      _exit(0); /*Exit child.*/
    }
  }
  for (i = 0; i < n; i++){ /*Killing childs with SIGUSR2 or SIGUSR1 according to their pid.*/
    if (pid[i] % 2 == 0){
      kill(pid[i], SIGUSR2); 
    }
    else{
      kill(pid[i], SIGUSR1);
    }
  }
  for (int i = 0; i < n; i++){ /*Wait for childs.*/
    wait(NULL);
  }
  for (int k = 0; k < n; k++){
    myResults[k]->pidID = pid[k]; /*Giving child id to struct.*/
  }
  parentOperation(n, myResults); /*Calling parentOperation to do last step.*/
}

void forksAnPipe(int n){
  int i;
  struct Output **myResults = (struct Output **)malloc(n * sizeof(struct Output *)); /*Malloc an array of struct.*/
  for (i = 0; i < n; i++){
    myResults[i] = malloc(sizeof(struct Output)); /*Malloc each node in the struct.*/
  }
  int fd[n][2]; /*Creating n number of pipes.*/
  for (i = 0; i < n; i++){
    if (pipe(fd[i]) == -1){ /* Piping them and printing error message if there is a problem.*/
      printf("Error with the pipe.\n");
      return;
    }
  }
  pid_t pid[n]; /*Creating n number of child.*/
  for (i = 0; i < n; i++){
    signal(SIGUSR2, signalHandlerUSR2); /*Calling signal handlers.*/
    signal(SIGUSR1, signalHandlerUSR1);
    if ((pid[i] = fork()) == 0){ /*Doing fork and child operations.*/
      srand(time(0) - i); /*Arranging srand to create random number for sleeping after sorting.*/
      struct Output *myData = malloc(sizeof(struct Output)); /*Creating a single node to fill it in childOperations instead of writing and output file.*/
      close(fd[i][0]); /*Close reading end of pipe.*/
      while (waitTillDay5) /*Infinite while loop till signal is coming.*/
        sleep(1);
      time(&signalTime); /*Get signal coming time.*/
      childOperationsForPipe(i, myData); /*Call childOperationsForPipe to do read inputs.*/
      if (write(fd[i][1], myData, sizeof(struct Output)) == -1){ /*Write the whole struct to pipe.*/
        printf("Error with the pipe write 1. \n"); /*Giving error message if there is any.*/
        return;
      }
      /*Here, I am writing also array because as I searched when we piped the struct dynamic arrays memory pointer
      couldn't be transfered clearly. Therefore, I am also piping the array to do not lose any data.*/
      if (write(fd[i][1], myData->numbers, myData->m * sizeof(int)) == -1){ /*Write the array of numbers to pipe.*/
        printf("Error with the pipe write 2. \n"); /*Giving error message if there is any.*/
        return;
      }
      close(fd[i][1]); /*Closing writing end of pipe.*/
      _exit(0); /*Exit child.*/
    }
  }
  for (i = 0; i < n; i++){ /*Killing childs with SIGUSR2 or SIGUSR1 according to their pid.*/
    if (pid[i] % 2 == 0){
      kill(pid[i], SIGUSR2);
    }
    else{
      kill(pid[i], SIGUSR1);
    }
  }
  for (i = 0; i < n; i++){ /*Getting information from pipes.*/
    close(fd[i][1]); /*Close writing end of pipe.*/
    struct Output *myDataFromPipe = malloc(sizeof(struct Output)); /*Creating a node to receive data.*/
    if (read(fd[i][0], myDataFromPipe, sizeof(struct Output)) < 0){ /*Reading first sended data which is struct.*/ 
      printf("error with the pipe read 1. \n"); /*Giving error message if there is any.*/
      return;
    }
    int *myNumbersFromPipe = malloc(myDataFromPipe->m * sizeof(int)); /*Dynamically allocating array to receive data.*/
    if (read(fd[i][0], myNumbersFromPipe, myDataFromPipe->m * sizeof(int)) < 0){ /*Reading second sended data which is array.*/
      printf("error with the pipe read 2. \n"); /*Giving error message if there is any.*/
      return;
    }
    myResults[i] = myDataFromPipe; /*Putting that receiving node to struct array.*/
    myResults[i]->numbers = myNumbersFromPipe; /* Putting array to the node.*/
    close(fd[i][0]); /*Closing reading end of pipe.*/
  }
  for (int i = 0; i < n; i++) { /*Wait for childs.*/
    wait(NULL);
  }
  for (int k = 0; k < n; k++){
    myResults[k]->pidID = pid[k]; /*Giving child id to struct.*/
  }
  parentOperationForPipe(n, myResults); /*Calling parentOperation to do last step.*/
}

void createFile(int m, int n){
  FILE *myFile; /*Creating a file.*/
  char filename[20]; /*A constant character array to arranging file name.*/
  int i, j;
  for (i = 0; i < n; i++){
    sprintf(filename, "input%d.txt", i); /*Creating n number of files with different names.*/
    myFile = fopen(filename, "w"); /*Opening file, if there is no file it creates it.*/
    fprintf(myFile, "%d\n", m); /*Putting m to first line of the file.*/
    for (j = 0; j < m; j++){ /*Writing m number of random integer to the file.*/
      fprintf(myFile, "%d ", rand() % m);
    }
    fclose(myFile); /*Closing the file.*/
  }
}

int swap(int *x, int *y){ /*It is basic swap function to use it later in selectionSort.*/
  int temp = *x;
  *x = *y;
  *y = temp;
}

int *selectionSort(int *myList, int size){
  int i, j, min;
  for (i = 0; i < size - 1; i++){
    min = i; /*Finding next minimum number.*/
    for (j = i + 1; j < size; j++){
      if (myList[j] < myList[min]){
        min = j;
      }
    } /*Swap that number.*/
    swap(&myList[min], &myList[i]);
  }
  return myList; /*Return the list.*/
}

int *insertionSort(int *myList, int size){
  int i, j, value;
  for (i = 0; i < size; i++){
    value = myList[i];
    for (j = i - 1; - 1 < j; j--){
      if (myList[j] > value){ /*Swaping the number while the position is higher.*/
        myList[j + 1] = myList[j];
      }
      else{
        break; /*Break it if it is not higher anymore.*/
      }
    }
    myList[j + 1] = value; /*Put the value at last position.*/
  }
  return myList; /*Return the list.*/
}

void signalHandlerUSR1(int signumber){
  waitTillDay5 = 0; /*Make global variable zero which will break infinite while loop.*/
}

void signalHandlerUSR2(int signumber){
  waitTillDay5 = 0; /*Make global variable zero which will break infinite while loop.*/
}

void childOperations(int fileNum){
  /*Defining variables.*/
  clock_t startTime, endTime;
  int i = 0, j;
  FILE *myFile;
  char filename[20], filename2[20];
  sprintf(filename, "input%d.txt", fileNum); /*Deciding file name according to fileNum which comes from child.*/
  myFile = fopen(filename, "r"); /*Open file to read.*/
  struct Output *myData = malloc(sizeof(struct Output)); /*Creating a node to fill it.*/
  if (myFile != NULL){
    if (fscanf(myFile, "%d\n", &myData->m) != EOF); /*Getting m.*/
    myData->numbers = (int *)malloc(myData->m * sizeof(int)); /*Malloc array according to m.*/
    for (j = 0; j < myData->m; j++){ /*Reading numbers into array.*/
      if (fscanf(myFile, "%d ", &myData->numbers[j]) != EOF);
    }
  }
  fclose(myFile); /*Close the file.*/
  int pidId = getpid(); /*Get pid of child.*/
  startTime = clock(); /*Start time.*/
  if (pidId % 2 == 0){ /*Doing sorts according to pid.*/
    myData->numbers = selectionSort(myData->numbers, myData->m);
  }
  else{
    myData->numbers = insertionSort(myData->numbers, myData->m);
  }
  endTime = clock(); /* End time.*/
  int randomSleepTime = rand() % 7 + 1; /*Getting a random sleep time.*/
  sleep(randomSleepTime); /*Sleep according to that.*/
  double execTime = (double)(endTime - startTime) / CLOCKS_PER_SEC + randomSleepTime; /*Calculating the execution time of sort.*/
  sprintf(filename2, "output%d.txt", fileNum); /*Creating an output file with same id of input file.*/
  myFile = fopen(filename2, "w"); /*Open or creating file to write.*/
  fprintf(myFile, "%d\n", myData->m); /*Writing m.*/
  for (j = 0; j < myData->m; j++){ /*Writing sorted numbers.*/
    fprintf(myFile, "%d ", myData->numbers[j]);
  }
  fprintf(myFile, "\n%lf\n", execTime); /*Writing execution time.*/
  if (pidId % 2 == 0){ /*Writing signal name and time according to pid.*/
    fprintf(myFile, "SIGUSR2 %s", ctime(&signalTime));
  }
  else{
    fprintf(myFile, "SIGUSR1 %s", ctime(&signalTime));
  }
  fclose(myFile); /*Close the file.*/
}

void childOperationsForPipe(int fileNum, struct Output *myOut){
  /*Defining variables.*/
  clock_t startTime, endTime;
  int i = 0, j;
  FILE *myFile;
  char filename[20];
  sprintf(filename, "input%d.txt", fileNum); /*Deciding file name according to fileNum which comes from child.*/
  myFile = fopen(filename, "r"); /*Open file to read.*/
  if (myFile != NULL){ /*Read datas into struct.*/
    if (fscanf(myFile, "%d\n", &myOut->m) != EOF);
    myOut->numbers = (int *)malloc(myOut->m * sizeof(int));
    for (j = 0; j < myOut->m; j++){
      if (fscanf(myFile, "%d ", &myOut->numbers[j]) != EOF);
    }
  }
  fclose(myFile); /*Close the file.*/
  char sigTime[25]; /*Creating a char array and get time of signal into it.*/
  strcpy(sigTime, ctime(&signalTime));
  int pidId = getpid(); /*Get pid.*/
  startTime = clock(); /*Start time.*/
  if (pidId % 2 == 0){ /*Doing sort according to pid of child.*/
    myOut->numbers = selectionSort(myOut->numbers, myOut->m);
  }
  else{
    myOut->numbers = insertionSort(myOut->numbers, myOut->m);
  }
  endTime = clock(); /*End time.*/
  int randomSleepTime = rand() % 7 + 1; /*Getting a random sleep time.*/
  sleep(randomSleepTime); /*Sleep according to that.*/
  double execTime = (double)(endTime - startTime) / CLOCKS_PER_SEC + randomSleepTime; /*Calculating the execution time of sort.*/
  myOut->execTime = execTime; /*Giving execution time to struct.*/
  char tempSignal[50]; /*Creating a temp signal variable.*/
  if (pidId % 2 == 0){ /*Deciding signal name according to pid of child and concatenate with time before giving it to struct.*/
    strcpy(tempSignal, "SIGUSR2 ");
    strcat(tempSignal, sigTime);
    strcpy(myOut->signal, tempSignal);
  }
  else{
    strcpy(tempSignal, "SIGUSR1 ");
    strcat(tempSignal, sigTime);
    strcpy(myOut->signal, tempSignal);
  }
}

void parentOperation(int n, struct Output **myResults){
  /*Defining variables.*/
  int i, j, min;
  double value;
  FILE *myFile;
  for (i = 0; i < n; i++){ /*Opening each output file and reading the data to corresponding struct variable.*/
    char filename[20];
    sprintf(filename, "output%d.txt", i);
    myFile = fopen(filename, "r");
    if (fscanf(myFile, "%d\n", &myResults[i]->m) != EOF); /*Getting m.*/
    myResults[i]->numbers = (int *)malloc(myResults[i]->m * sizeof(int)); /*Dynamically allocate array.*/
    for (j = 0; j < myResults[i]->m; j++){
      if (fscanf(myFile, "%d ", &myResults[i]->numbers[j]) != EOF); /*Getting numbers.*/
    }
    if (fscanf(myFile, "\n%lf\n", &myResults[i]->execTime) != EOF); /*Getting execution time.*/
    while (fgets(myResults[i]->signal, 50, myFile)); /*Getting signal name and time.*/
    fclose(myFile);
  }
  struct Output *myTemp;
  for (i = 0; i < n - 1; i++){ /*Sorting outputs according to execution time.*/
    min = i;
    for (j = i + 1; j < n; j++){
      if (myResults[j]->execTime < myResults[min]->execTime){
        min = j;
      }
    }
    myTemp = myResults[i];
    myResults[i] = myResults[min];
    myResults[min] = myTemp;
  }
  myFile = fopen("output.txt", "w"); /*Open a final output file.*/
  for (i = 0; i < n; i++){ /*Write the data into it.*/
    fprintf(myFile, "%d %lf ", myResults[i]->pidID, myResults[i]->execTime); /*I prefer the write pidID of childs also to see it. However, of course we can easily remove it to if it is unnecessary.*/
    for (j = 0; j < myResults[i]->m; j++){
      fprintf(myFile, "%d ", myResults[i]->numbers[j]);
    }
    fprintf(myFile, "%s\n", myResults[i]->signal);
  }
  fclose(myFile); /*Close the file.*/
}

void parentOperationForPipe(int n, struct Output **myResults){
  /*Defining variables.*/
  int i, j;
  double value;
  int min;
  struct Output *myTemp;
  for (i = 0; i < n - 1; i++){ /*Sorting outputs according to execution time.*/
    min = i;
    for (j = i + 1; j < n; j++){ 
      if (myResults[j]->execTime < myResults[min]->execTime){
        min = j;
      }
    }
    myTemp = myResults[i];
    myResults[i] = myResults[min];
    myResults[min] = myTemp;
  }
  FILE *myFile;
  myFile = fopen("output.txt", "w");  /*Open a final output file.*/
  for (i = 0; i < n; i++){ /*Write the data into it.*/
    fprintf(myFile, "%d %lf ", myResults[i]->pidID, myResults[i]->execTime); /*I prefer the write pidID of childs also to see it. However, of course we can easily remove it to if it is unnecessary.*/
    for (j = 0; j < myResults[i]->m; j++){
      fprintf(myFile, "%d ", myResults[i]->numbers[j]);
    }
    fprintf(myFile, "%s", myResults[i]->signal);
  }
  fclose(myFile); /*Close the file.*/
}
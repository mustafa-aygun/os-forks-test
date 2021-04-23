#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

int *selectionSort(int[],int);
int *insertionSort(int[],int);
void createFile(int, int);

int main(int argc, char const *argv[])
{   
    srand(time(0));
    int n,m;
    printf("Welcome to app\n");
    printf("Enter number of file: ");
    scanf("%d",&n);
    printf("Enter size of numbers: ");
    scanf("%d",&m);
    
    createFile(m,n);
    
    return 0;
}

void createFile(int m, int n){
    FILE *myFile;
    char filename[20]; 
    int i,j;
    for(i = 0; i < n; i++){

        sprintf(filename,"input%d.txt",i);
        myFile = fopen(filename, "w");
        fprintf(myFile,"%d\n",m);
        for(j = 0; j < m; j++){
            fprintf(myFile,"%d ",rand()%m);
        }
    }
}

int swap(int *x, int *y){
    int temp = *x;
    *x = *y;
    *y = temp;
}

int *selectionSort(int *mylist, int size){

    int i,j,min;
    for (i = 0; i < size-1; i++)
    {
        min = i;
        for(j = i+1; j < size; j++){          
            if( mylist[j] < mylist[min]){
                min = j;
            }
        }
        swap(&mylist[min],&mylist[i]);
    }

    return mylist;
}

int *insertionSort(int *myList, int size){

    int i, j,value;
    for(i = 0; i < size; i++){
        
        value = myList[i];
        for(j = i-1; -1 < j; j--){
            if(myList[j] > value){
                myList[j+1] = myList[j];

            }
            else{
                break;
            }
        }
        myList[j+1] = value;
    }
    return myList;
}


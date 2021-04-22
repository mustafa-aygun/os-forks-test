#include<stdio.h>
#include<stdlib.h>

int *selectionSort(int[],int);
int *insertionSort(int[],int);

int main(int argc, char const *argv[])
{   
    int n;
    printf("Welcome to app\n");
    printf("Enter n: ");
    scanf("%d",&n);
    int *mylist = (int*)malloc(n*sizeof(int));
    mylist[0] = 3;
    mylist[1] = 9;
    mylist[2] = 5;
    mylist[3] = 4;
    mylist[4] = 6;
    
    mylist = insertionSort(mylist,n);
    for (int i = 0; i < n; i++)
    {
        printf("%d",mylist[i]);
    }
    
    return 0;
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


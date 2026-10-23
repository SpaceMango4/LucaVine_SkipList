#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct node{
    int value;
    struct node* next;
}node;

typedef struct expressNode{
    int level;
    int value;
    struct expressNode* next;
    struct expressNode* below;

    //Only if Level = 1 will this be set
    //For all other levels, they will just have a next and below of type expressNode
    struct node* localListNode;
}expNode;

int* inputArr(int* len);
void merge(int arr[], int left, int mid, int right);
void mergeSort(int arr[], int left, int right);
node* initializeLocalList(int* arr, int* len);
expNode** expressLists(node* head);
int coinFlip();

int main(){
    srand(time(NULL));
    int len;
    int* arr = inputArr(&len);
    node* head = initializeLocalList(arr, &len);
    node* temp = head;

    printf("Current values in list: \n");
    while(head != NULL){
        printf("%d ", head->value);
        head = head->next;
    }
    head = temp;

    printf("\n\nDisplaying Express Lists:\n\n");
    expNode** setOfExpressLists = expressLists(head);
    int index = 0;
    while(setOfExpressLists[index] != NULL){
        expNode* head = setOfExpressLists[index];
        printf("\nExpress List #%d:\n", index+1);
        while(head != NULL){
            printf("%d ", head->value);
            head = head->next;
        }
        index++;
    }


    free(arr);
    //free memory for future functions/operations
    printf("\n");
    return 0;
}

int* inputArr(int* len){
    printf("How many elements will the linked list start with?\n");
    scanf("%d", len);

    int* arr = malloc((*len) * sizeof(int));
    
    for (int i=0; i<(*len); i++){
        printf("\nEnter element #%d ", i+1);
        scanf(" %d", arr+i);
    }

    mergeSort(arr, 0, (*len)-1);
    
    return arr;
}


node* initializeLocalList(int* arr, int* len){
    if (*len <= 0) return NULL;

    node* head = malloc(sizeof(node));
    node* curr = head;

    for (int i=0; i<(*len)-1; i++){
        curr->value = arr[i];
        curr->next = malloc(sizeof(node));
        curr = curr->next;
    }
    curr->value = arr[(*len)-1];
    curr->next = NULL;


    return head;
}

expNode** expressLists(node* head){
    //indicates number of expressLists/levels
    int levels = 0;
    expNode** expLists = malloc(sizeof(expNode*) * levels);

    //for each value in localList flipcoin to build height (how many levels of expressLists)
    while(head != NULL){
        int currHeight = 0;
        expNode* tempBelow = NULL;
        while(coinFlip()){
            currHeight++;
            //If no expList current exists for this level
            if (currHeight > levels){
                levels++;
                expLists = realloc(expLists, sizeof(expNode*) * levels);


                expLists[currHeight-1] = malloc(sizeof(expNode));
                expNode* expHead = expLists[currHeight-1];
                expHead->value = head->value;
                expHead->level = currHeight;

                //If its the first level, assign the localListNode.
                if(currHeight == 1) {
                    expHead->localListNode = head;
                    expHead->below = NULL;
                }
                else{
                    expHead->localListNode = NULL;
                    expHead->below = tempBelow;
                }

                expHead->next = NULL;
                tempBelow = expHead;
            }
            else{
                expNode* expHead = expLists[currHeight-1];

                //Traverse to end node of list at this level
                while (expHead->next != NULL) expHead = expHead->next;

                expHead->next = malloc(sizeof(expNode));
                expHead = expHead->next;
                expHead->value = head->value;
                expHead->level = currHeight;

                if(currHeight == 1){
                    expHead->localListNode = head;
                    expHead->below = NULL;
                }
                else{
                    expHead->localListNode = NULL;
                    expHead->below = tempBelow;
                }

                expHead->next = NULL;
                tempBelow = expHead;

            }
        }

        head = head->next;
    }

    //Add null pointer to end of array to signify termination
    expLists = realloc(expLists, sizeof(expNode*) * (levels+1));
    expLists[levels] = NULL;

    return expLists;
}

int coinFlip(){
    return (rand() % 2);
}

//Mergesort functions:
void mergeSort(int arr[], int left, int right){
    if (left >= right) return;
    
    int mid = (right+left)/2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid+1, right);
    merge(arr, left, mid, right);
    return;
}

void merge(int arr[], int left, int mid, int right){
    int left_arr_size = mid - left +1;
    int right_arr_size = right-mid;
    int* L = (int *)malloc(left_arr_size * sizeof(int));
    int* R = (int *)malloc(right_arr_size * sizeof(int));
    
    for (int i = 0; i < left_arr_size; i++){
        L[i] = arr[left + i];
    }
    for (int i = 0; i < right_arr_size; i++){
        R[i] = arr[mid + 1 + i];
    }
    
    int i = 0; //Index for left array
    int j = 0; //Index for right array
    int k = left; //Index for original array (merged)
    while (i < left_arr_size && j < right_arr_size){
        if(L[i] <= R[j]){
            arr[k] = L[i];
            k++;
            i++;
        }
        else{
            arr[k] = R[j];
            k++;
            j++;
        }
    }
    while (i < left_arr_size){
        arr[k] = L[i];
        k++;
        i++;
    }
    while (j < right_arr_size){
        arr[k] = R[j];
        k++;
        j++;
    }
    free(L);
    free(R);
    return;
}




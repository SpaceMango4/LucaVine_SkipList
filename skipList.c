#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

//Node struct
typedef struct node{
    int value;
    struct node* next;
}node;

//Express node struct
typedef struct expressNode{
    int level;
    int value;
    struct expressNode* next;
    struct expressNode* below;

    //Only if Level = 1 will this be set
    //For all other levels, they will just have a next and below of type expressNode
    struct node* localListNode;
}expNode;

//Skip list struct
typedef struct skipList{
    int levels;
    expNode** expressLists;
    node* localList;
} skipList;


//Function declarations:
int* inputArr(int* len);
void merge(int arr[], int left, int mid, int right);
void mergeSort(int arr[], int left, int right);
node* initializeLocalList(int* arr, int* len);
expNode** expressLists(node* head, int* levels);
void search(skipList sL, int n);
void insert(expNode** expLists, int n);
void delete(expNode** expLists, int n);
int coinFlip();

int main(){
    //Seed random for coinflip function
    srand(time(NULL));

    //Get input array from user then created linked list out of it for local list
    int len;
    int* arr = inputArr(&len);
    node* localListHead = initializeLocalList(arr, &len);
    node* temp = localListHead;

    //Printing current values in list (sorted) after user inputs
    printf("\nCurrent values in list: \n");
    while(localListHead != NULL){
        printf("%d ", localListHead->value);
        localListHead = localListHead->next;
    }
    localListHead = temp;

    //Create express lists
    int levels = 0;
    expNode** setOfExpressLists = expressLists(localListHead, &levels);
    skipList sL = {levels, setOfExpressLists, localListHead};


    //Print values in express lists
    printf("\n\nDisplaying Express Lists:\n");
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
    
    char cont;
    printf("\n\nDo you want to search? (y/n)\n");
    getchar();
    scanf("%c", &cont);
    while(cont == 'y' || cont == 'Y'){
        printf("\nWhat number are you trying to find?\n");
        int n;
        getchar();
        scanf("%d", &n);

        search(sL, n);

        printf("\n\nDo you want to search? (y/n)\n");
        getchar();
        scanf("%c", &cont);
    }

    free(arr);
    //free(setOfExpressLists); --- need to loop amongst pointers and free each one
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

expNode** expressLists(node* head, int* levelsToUpdate){
    //Indicates number of expressLists/levels
    int levels = 0;
    expNode** expLists = malloc(sizeof(expNode*) * levels);

    //For each value in localList flipcoin to build height (how many levels of expressLists)
    while(head != NULL){
        int currHeight = 0;
        expNode* tempBelow = NULL;
        while(coinFlip()){
            currHeight++;
            //If no expList exists for this level
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

    //Update levels
    *levelsToUpdate = levels;

    return expLists;
}

int coinFlip(){
    return (rand() % 2);
}

void insert(expNode** expLists, int n){
    //To be implemented
    return;
}

void search(skipList sL, int n){
    //If there are no express lists:
        if (sL.levels == 0){
            node* curr = sL.localList;

            while(curr != NULL && curr->value <= n){
                if(curr->value == n){
                    printf("%d exists!", n);
                    return;
                }

                curr = curr->next;
            }

            printf("%d does not exist!", n);
            return;
        }

    //Start at top list
    expNode* currNode = sL.expressLists[sL.levels - 1];
    expNode* nextNode = currNode;
    expNode* prev = NULL;

    for(int i=sL.levels-1; i>=0; i--){

        //Start at head for current express list
        currNode = nextNode;
        if(currNode == NULL) break;
        prev = NULL;  // Reset prev for this level

        //3 Cases: 1. n = node_val 2. n < node_val 3. n > node_val

        //Case 1:
        if (n == currNode->value){
            printf("%d exists!", n);
            return;
        }

        //Case 2:
        else if(n < currNode->value){
            //If we are at the first level & at head of first express level, we just search the local list from its head to first node with val>=n
            if (i == 0 && prev == NULL){
                node* localListHead = sL.localList;
                do{
                    if (localListHead->value == n){
                        printf("%d exists!", n);
                        return;
                    }
                    localListHead = localListHead->next;
                } while(localListHead != NULL && localListHead->value <= n);

                printf("%d does not exist!", n);
                return;
            }
            //If we are at the first level, but not head of first express level
            else if (i==0){
                node* localListHead = prev->localListNode;
                while(localListHead != NULL && localListHead->value <= n){
                    if (localListHead->value == n){
                        printf("%d exists!", n);
                        return;
                    }
                    localListHead = localListHead->next;
                }

                printf("%d does not exist!", n);
                return;
            }
            //For all other levels
            else{
                //If we are at head of current express list, we start from head of next express list
                if (prev == NULL){
                    nextNode = sL.expressLists[i-1];
                    continue;
                }
                else{
                    nextNode = prev->below;
                    continue;
                }
            }
        }

        //Case 3:
        else if(n > currNode->value){
            //If we are at level 1:
            if (i == 0){
                while(currNode != NULL && n >= currNode->value){
                    if (n == currNode->value){
                        printf("%d exists!", n);
                        return;
                    }
                    prev = currNode;
                    currNode = currNode->next;
                }
                //Now traverse local list
                node* localListNode = prev->localListNode;
                while(localListNode != NULL && n >= localListNode->value){
                    if(n == localListNode->value){
                        printf("%d exists!", n);
                        return;
                    }

                    localListNode = localListNode->next;
                }
                printf("%d does not exist!", n);
                return;
            }

            //For all other levels:
            else{
                while(currNode->next != NULL && currNode->next->value <= n){
                    prev = currNode;
                    currNode = currNode->next;
                }
                if (currNode->value == n){
                    printf("%d exists!", n);
                    return;
                }

                nextNode = currNode->below;
                continue;
            }
        }
    }

    printf("%d doesnt exist", n);
    return;
}

void delete(expNode** expLists, int n){
    //To be implemented
    return;
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




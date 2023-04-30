#include <stdio.h>
#include <stdlib.h>

struct ProcessStruct {
    int id;
    int arrivalTime;
    int priority;
    int runningTime;
    int remainingTime;
    int startTime;
    int waitingTime;
    int lastStopedTime;
    int pid;
    int memSize;
};

struct TreeNode {
    struct ProcessStruct* data;
    struct TreeNode* left, *right;
    int size;
    int splitted;
};

struct TreeNode* createTreeNode(int size) {
    struct TreeNode* n = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    n->data = NULL;
    n->left = NULL;
    n->right = NULL;
    n->size = size;
    int splitted = 0;
    return n;
}

struct TreeNode* buddyMemoryNode; 


int allocateProcessMemoryBuddyRec(struct TreeNode* currNode, struct ProcessStruct* p, int left, int right) {

    if(p->memSize > currNode->size) {
        return -1;
    }
    
    if((currNode->data != NULL && currNode->data->id != -1)) {
        return -1;
    }

    if(currNode->size / 2 >= p->memSize) {
        int mid = (right + left) / 2;

        // try left
        if(currNode->left == NULL) {
            currNode->left = createTreeNode(currNode->size / 2);
        }

        int try_left = -1;

        try_left = allocateProcessMemoryBuddyRec(currNode->left, p, left, mid);

        if(try_left == 1) {
            currNode->data = (struct ProcessStruct *)malloc(sizeof(struct ProcessStruct));
            currNode->data->id = -1;
            return 1;
        }

        int try_right = -1;
        if(currNode->right == NULL) {
            currNode->right = createTreeNode(currNode->size / 2);
        }

        try_right = allocateProcessMemoryBuddyRec(currNode->right, p, mid, right);
        if(try_right == 1) {
            if(currNode->data != NULL) {
                currNode->data = (struct ProcessStruct *)malloc(sizeof(struct ProcessStruct));
                currNode->data->id = -1;
            }
            return 1;
        }
        return -1;
    } else if(currNode->data == NULL) {
        currNode->data = p;
        printf("the process %d settled from %d to %d size %d\n", p->id, left, right - 1, currNode->size);
        return 1;
    } else {
        printf("error no space in the TreeNode %d %d\n", p->id, currNode->size);
        return -1;
    }
}

int allocateProcessMemoryBuddy(struct ProcessStruct *process){
    return allocateProcessMemoryBuddyRec(buddyMemoryNode, process, 0, 1024);
}

struct TreeNode* deallocateProcessMemoryBuddyRec(struct TreeNode* currNode, int id, int left, int right) {
    if(currNode == NULL) {
        return NULL;
    }

    printf("Trying to remove id %d current node has %d\n", id, currNode->data->id);

    int mid = (right + left) / 2;

    if(currNode->data != NULL && currNode->data->id == id) {
        printf("the process %d removed from %d to %d size %d\n", currNode->data->id, left, right - 1, currNode->size);
        free(currNode->data);
        currNode->data = NULL;
        if(currNode->left == NULL && currNode->right == NULL) {
            free(currNode);
            currNode = NULL;
            return NULL;
        } else if(currNode->left == NULL) {
            struct TreeNode* temp = currNode->right;
            free(currNode);
            currNode = temp;
        } else if(currNode->right == NULL) {
            struct TreeNode* temp = currNode->left;
            free(currNode);
            currNode = temp;
        } else {
            struct TreeNode* leftmost = currNode->left;
            while(leftmost->right != NULL) {
                leftmost = leftmost->right;
            }
            currNode->data = leftmost->data;
            free(leftmost);
        }
        return currNode;
    } else {
        currNode->left = deallocateProcessMemoryBuddyRec(currNode->left, id, left, mid);
        currNode->right = deallocateProcessMemoryBuddyRec(currNode->right, id, mid, right);

        if(currNode->left == NULL && currNode->right == NULL && currNode->data->id == -1) {
            free(currNode);
            currNode = NULL;
            return NULL;
        }
        return currNode;
    }
}

struct TreeNode * deallocateProcessMemoryBuddy(struct ProcessStruct *process){
    return deallocateProcessMemoryBuddyRec(buddyMemoryNode, process->id, 0, 1024);
}


int main() {
    buddyMemoryNode = createTreeNode(1024);

    struct ProcessStruct * p1 = (struct ProcessStruct*) malloc(sizeof(struct ProcessStruct));
    p1->id = 1;
    p1->memSize = 150;
    allocateProcessMemoryBuddy(p1);

    struct ProcessStruct * p2 = (struct ProcessStruct*) malloc(sizeof(struct ProcessStruct));
    p2->id = 2;
    p2->memSize = 200;
    allocateProcessMemoryBuddy(p2);

    struct ProcessStruct * p3 = (struct ProcessStruct*) malloc(sizeof(struct ProcessStruct));
    p3->id = 3;
    p3->memSize = 250;
    allocateProcessMemoryBuddy(p3);

    struct ProcessStruct * p4 = (struct ProcessStruct*) malloc(sizeof(struct ProcessStruct));
    p4->id = 4;
    p4->memSize = 425;
    allocateProcessMemoryBuddy(p4);

    deallocateProcessMemoryBuddy(p1);
    allocateProcessMemoryBuddy(p4);
    deallocateProcessMemoryBuddy(p3);
    allocateProcessMemoryBuddy(p4);
    deallocateProcessMemoryBuddy(p2);
    allocateProcessMemoryBuddy(p4);

    return 0;
}

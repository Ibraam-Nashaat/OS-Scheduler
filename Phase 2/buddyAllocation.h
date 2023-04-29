#include <stdio.h>
#include <stdlib.h>

struct TreeNode
{
    struct ProcessStruct *data;
    struct TreeNode *left, *right;
    int memSize;
    int splitted;
};

struct TreeNode *createTreeNode(int memSize)
{
    struct TreeNode *n = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    n->data = NULL;
    n->left = NULL;
    n->right = NULL;
    n->memSize = memSize;
    return n;
}

struct TreeNode *buddyMemoryNode;

int addToBuddyMemory(struct TreeNode *currNode, struct ProcessStruct *p, int left, int right)
{

    if (p->memSize > currNode->memSize)
    {
        return -1;
    }

    if (currNode->memSize / 2 >= p->memSize)
    {
        int mid = (right + left) / 2;

        // try left
        if (currNode->left == NULL)
            currNode->left = createTreeNode(currNode->memSize / 2);
        // printf("the Process %d went left %d\n",p->id , currNode->memSize);

        int try_left = -1;
        if (currNode->left->data == NULL)
            try_left = addToBuddyMemory(currNode->left, p, left, mid);

        if (try_left == 1)
            return 1;

        // printf("fault in left %d\n", currNode->memSize);
        //  try right

        int try_right = -1;
        if (currNode->right == NULL)
            currNode->right = createTreeNode(currNode->memSize / 2);
        // printf("the Process %d went right %d\n",p->id ,currNode->memSize);

        if (currNode->right->data == NULL)
            try_right = addToBuddyMemory(currNode->right, p, mid, right);
        if (try_right == 1)
            return 1;
        // return -1
        // printf("fault in right %d\n", currNode->memSize);

        // printf("error no space in the TreeNode %d %d\n",p->id, currNode->memSize);
        return 0;
    }
    else if (currNode->data == NULL)
    {
        currNode->data = p;
        printf("the Process %d settled from %d to %d size %d\n", p->id, left, right - 1, currNode->memSize);
        return 1;
    }
}

struct TreeNode *deleteFromBuddyMemory(struct TreeNode *currNode, int id, int left, int right)
{
    if (currNode == NULL)
        return NULL;

    int mid = (right + left) / 2;

    if (currNode->data != NULL && currNode->data->id == id)
    {
        printf("the Process %d removed from %d to %d size %d\n", currNode->data->id, left, right - 1, currNode->memSize);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
        return NULL;
    }
    else
    {
        currNode->left = deleteFromBuddyMemory(currNode->left, id, left, mid);
        currNode->right = deleteFromBuddyMemory(currNode->right, id, mid, right);

        if (currNode->left == NULL && currNode->right == NULL && currNode->data == NULL)
        {
            free(currNode);
            currNode = NULL;
            return NULL;
        }
        return currNode;
    }
}

/* int main()
{
    memory = createTreeNode(1024);

    struct Process * p1 = (struct Process*) malloc(sizeof(struct Process));
    p1->id = 1;
    p1->memSize = 200;
    addToBuddyMemory(memory,p1 ,0, 1024);

    struct Process * p2 = (struct Process*) malloc(sizeof(struct Process));
    p2->id = 2;
    p2->memSize = 200;
    addToBuddyMemory(memory,p2 ,0, 1024);

    struct Process * p3 = (struct Process*) malloc(sizeof(struct Process));
    p3->id = 3;
    p3->memSize = 200;
    addToBuddyMemory(memory,p3, 0, 1024);

    struct Process * p4 = (struct Process*) malloc(sizeof(struct Process));
    p4->id = 4;
    p4->memSize = 100;
    addToBuddyMemory(memory, p4, 0, 1024);

    struct Process * p5 = (struct Process*) malloc(sizeof(struct Process));
    p5->id = 5;
    p5->memSize = 100;
    addToBuddyMemory(memory, p5, 0, 1024);

    deleteFromBuddyMemory(memory, p1->id, 0, 1024);
    deleteFromBuddyMemory(memory, 2, 0, 1024);

    p1 = (struct Process*) malloc(sizeof(struct Process));
    p1->id = 1;
    p1->memSize = 200;
    addToBuddyMemory(memory,p1 ,0, 1024);

    p2 = (struct Process*) malloc(sizeof(struct Process));
    p2->id = 2;
    p2->memSize = 200;
    addToBuddyMemory(memory,p2 ,0, 1024);

    return 0;
}  */
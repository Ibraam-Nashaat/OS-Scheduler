struct TreeNode
{
    struct TreeNode *left, *right;
    int size;
    int isSplitted;
    int hasData;
    int processId;
    int processMemSize;
};

struct TreeNode *createTreeNode(int size)
{
    struct TreeNode *n = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    n->left = NULL;
    n->right = NULL;
    n->size = size;
    n->isSplitted = 0;
    n->hasData = 0;
    n->processId = -1;
    n->processMemSize = -1;

    return n;
}

struct TreeNode *buddyMemoryNode;

int allocateProcessMemoryBuddyRec(struct TreeNode *currNode, struct ProcessStruct *p, int left, int right)
{

    if (p->memSize > currNode->size)
    {
        return 0;
    }

    if ((currNode->hasData == 1 && currNode->isSplitted == 0))
    {
        return 0;
    }

    if (currNode->size / 2 >= p->memSize)
    {
        int mid = (right + left) / 2;

        // try left
        if (currNode->left == NULL)
        {
            currNode->left = createTreeNode(currNode->size / 2);
        }

        int try_left = -1;

        try_left = allocateProcessMemoryBuddyRec(currNode->left, p, left, mid);

        if (try_left == 1)
        {
            currNode->hasData = 1;
            currNode->isSplitted = 1;
            return 1;
        }

        int try_right = -1;
        if (currNode->right == NULL)
        {
            currNode->right = createTreeNode(currNode->size / 2);
        }

        try_right = allocateProcessMemoryBuddyRec(currNode->right, p, mid, right);
        if (try_right == 1)
        {
            if (currNode->hasData == 1)
            {
                currNode->hasData = 1;
                currNode->isSplitted = 1;
            }
            return 1;
        }
        return 0;
    }
    else if (currNode->hasData == 0)
    {
        currNode->hasData = 1;
        currNode->processId = p->id;
        currNode->processMemSize = p->memSize;
        printf("the process %d settled from %d to %d size %d\n", p->id, left, right - 1, currNode->size);
        logMemoryAllocation(getClk(), currNode->processMemSize, currNode->processId, left, right - 1);
        return 1;
    }
    else
    {
        printf("error no space in the TreeNode %d %d\n", p->id, currNode->size);
        return 0;
    }
}

int allocateProcessMemoryBuddy(struct ProcessStruct *process)
{
    return allocateProcessMemoryBuddyRec(buddyMemoryNode, process, 0, MEMORY_SIZE);
}

struct TreeNode *deallocateProcessMemoryBuddyRec(struct TreeNode *currNode, int id, int left, int right)
{
    if (currNode == NULL)
    {
        return NULL;
    }

    int mid = (right + left) / 2;

    if (currNode->hasData == 1 && currNode->processId == id)
    {
        printf("the process %d removed from %d to %d size %d\n", currNode->processId, left, right - 1, currNode->size);
        logMemoryDeallocation(getClk(), currNode->processMemSize, currNode->processId, left, right - 1);
        currNode->hasData = 0;
        if (currNode->left == NULL && currNode->right == NULL)
        {
            free(currNode);
            currNode = NULL;
            return NULL;
        }
        else if (currNode->left == NULL)
        {
            struct TreeNode *temp = currNode->right;
            free(currNode);
            currNode = temp;
        }
        else if (currNode->right == NULL)
        {
            struct TreeNode *temp = currNode->left;
            free(currNode);
            currNode = temp;
        }
        else
        {
            struct TreeNode *leftmost = currNode->left;
            while (leftmost->right != NULL)
            {
                leftmost = leftmost->right;
            }
            currNode->processId = leftmost->processId;
            currNode->processMemSize = leftmost->processMemSize;
            currNode->isSplitted = leftmost->isSplitted;
            free(leftmost);
        }
        return currNode;
    }
    else
    {
        currNode->left = deallocateProcessMemoryBuddyRec(currNode->left, id, left, mid);
        currNode->right = deallocateProcessMemoryBuddyRec(currNode->right, id, mid, right);

        if (currNode->left == NULL && currNode->right == NULL && currNode->isSplitted == 1)
        {
            free(currNode);
            currNode = NULL;
            return NULL;
        }
        return currNode;
    }
}

struct TreeNode *deallocateProcessMemoryBuddy(struct ProcessStruct *process)
{
    return deallocateProcessMemoryBuddyRec(buddyMemoryNode, process->id, 0, MEMORY_SIZE);
}
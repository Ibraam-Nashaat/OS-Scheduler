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

int allocateProcessMemoryBuddy(struct TreeNode *currNode, struct ProcessStruct *p, int left, int right)
{

    if (p->memSize > currNode->memSize)
    {
        return -1;
    }

    if (currNode->memSize / 2 >= p->memSize)
    {
        int mid = (right + left) / 2;

        if (currNode->left == NULL)
            currNode->left = createTreeNode(currNode->memSize / 2);

        int try_left = -1;
        if (currNode->left->data == NULL)
            try_left = allocateProcessMemoryBuddy(currNode->left, p, left, mid);

        if (try_left == 1)
            return 1;

        int try_right = -1;
        if (currNode->right == NULL)
            currNode->right = createTreeNode(currNode->memSize / 2);

        if (currNode->right->data == NULL)
            try_right = allocateProcessMemoryBuddy(currNode->right, p, mid, right);
        if (try_right == 1)
            return 1;
        return 0;
    }
    else if (currNode->data == NULL)
    {
        currNode->data = p;
        printf("the Process %d settled from %d to %d size %d\n", p->id, left, right - 1, currNode->memSize);
        return 1;
    }
}

struct TreeNode *deallocateProcessMemoryBuddy(struct TreeNode *currNode, int id, int left, int right)
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
        currNode->left = deallocateProcessMemoryBuddy(currNode->left, id, left, mid);
        currNode->right = deallocateProcessMemoryBuddy(currNode->right, id, mid, right);

        if (currNode->left == NULL && currNode->right == NULL && currNode->data == NULL)
        {
            free(currNode);
            currNode = NULL;
            return NULL;
        }
        return currNode;
    }
}
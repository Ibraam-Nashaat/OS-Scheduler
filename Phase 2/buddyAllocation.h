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

int allocateProcessMemoryBuddyRec(struct TreeNode *node, struct ProcessStruct *process, int leftBound, int rightBound)
{
    if (process->memSize > node->memSize)
    {
        return 0; // Memory allocation failed
    }

    if (node->memSize / 2 >= process->memSize)
    {
        int midPoint = (rightBound + leftBound) / 2;

        if (node->left == NULL)
            node->left = createTreeNode(node->memSize / 2);

        int leftSubtreeAllocation = 0;
        if (node->left->data == NULL)
            leftSubtreeAllocation = allocateProcessMemoryBuddyRec(node->left, process, leftBound, midPoint);

        if (leftSubtreeAllocation == 1)
            return 1; // Memory allocation successful

        if (node->right == NULL)
            node->right = createTreeNode(node->memSize / 2);

        int rightSubtreeAllocation = 0;
        if (node->right->data == NULL)
            rightSubtreeAllocation = allocateProcessMemoryBuddyRec(node->right, process, midPoint, rightBound);

        if (rightSubtreeAllocation == 1)
            return 1; // Memory allocation successful

        return 0; // Memory allocation failed
    }
    else if (node->data == NULL)
    {
        node->data = process;
        printf("The process %d settled from %d to %d size %d\n", node->data->id, leftBound, rightBound - 1, node->memSize);
        return 1; // Memory allocation successful
    }

    return 0; // Memory allocation failed
}

int allocateProcessMemoryBuddy(struct ProcessStruct *process){
    return allocateProcessMemoryBuddyRec(buddyMemoryNode, process, 0, MEMORY_SIZE);
}

struct TreeNode *deallocateProcessMemoryBuddyRec(struct TreeNode *currNode, int id, int left, int right)
{
    if (currNode == NULL)
        return NULL;

    int mid = (right + left) / 2;

    if (currNode->data != NULL && currNode->data->id == id)
    {
        printf("the Process %d removed from %d to %d size %d\n", currNode->data->id, left, right - 1, currNode->memSize);
        free(currNode->data);
        currNode->data = NULL;

        if (currNode->left == NULL && currNode->right == NULL) {
            free(currNode);
            return NULL;
        }

        if (currNode->left == NULL) {
            struct TreeNode *newCurrNode = currNode->right;
            free(currNode);
            return newCurrNode;
        }

        if (currNode->right == NULL) {
            struct TreeNode *newCurrNode = currNode->left;
            free(currNode);
            return newCurrNode;
        }

        // both children are non-NULL
        struct TreeNode *newCurrNode = currNode->right;
        while (newCurrNode->left != NULL)
            newCurrNode = newCurrNode->left;
        newCurrNode->left = currNode->left;
        free(currNode);
        return newCurrNode;
    }
    else
    {
        currNode->left = deallocateProcessMemoryBuddyRec(currNode->left, id, left, mid);
        currNode->right = deallocateProcessMemoryBuddyRec(currNode->right, id, mid, right);

        if (currNode->left == NULL && currNode->right == NULL && currNode->data == NULL)
        {
            free(currNode);
            return NULL;
        }
        return currNode;
    }
}

struct TreeNode * deallocateProcessMemoryBuddy(struct ProcessStruct *process){
    return deallocateProcessMemoryBuddyRec(buddyMemoryNode, process->id, 0, MEMORY_SIZE);
}
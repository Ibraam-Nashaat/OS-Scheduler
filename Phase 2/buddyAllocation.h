struct TreeNode
{
    struct ProcessStruct *data;
    struct TreeNode *left, *right;
    int nodeSize;
};

struct TreeNode *createTreeNode(int nodeSize)
{
    struct TreeNode *n = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    n->data = NULL;
    n->left = NULL;
    n->right = NULL;
    n->nodeSize = nodeSize;
    return n;
}

struct TreeNode *buddyMemoryNode;

int allocateProcessMemoryBuddyRec(struct TreeNode *node, struct ProcessStruct *process, int leftBound, int rightBound)
{
    if (process->memSize > node->nodeSize)
    {
        return 0; // Memory allocation failed
    }

    if((node->data != NULL && node->data->id != -1))
        return 0;

    if (node->nodeSize / 2 >= process->memSize)
    {
        int midPoint = (rightBound + leftBound) / 2;

        if (node->left == NULL)
            node->left = createTreeNode(node->nodeSize / 2);

        int leftSubtreeAllocation = allocateProcessMemoryBuddyRec(node->left, process, leftBound, midPoint);

        if (leftSubtreeAllocation == 1){
            node->data = (struct ProcessStruct *) malloc(sizeof(struct ProcessStruct));
            node->data->id = -1;
            return 1; // Memory allocation successful
        }

        if (node->right == NULL)
            node->right = createTreeNode(node->nodeSize / 2);

        int rightSubtreeAllocation = allocateProcessMemoryBuddyRec(node->right, process, midPoint, rightBound);

        if (rightSubtreeAllocation == 1){
            if(node->data != NULL){
                node->data = (struct ProcessStruct *) malloc(sizeof(struct ProcessStruct));
                node->data->id = -1;
            }
            return 1; // Memory allocation successful
        }

        return 0; // Memory allocation failed
    }
    else if (node->data == NULL)
    {
        node->data = process;
        printf("The process %d settled from %d to %d size %d\n", node->data->id, leftBound, rightBound - 1, node->nodeSize);
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
        printf("the process %d removed from %d to %d size %d\n", currNode->data->id, left, right - 1, currNode->nodeSize);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
        return NULL;
    }
    else
    {
        currNode->left = deallocateProcessMemoryBuddyRec(currNode->left, id, left, mid);
        currNode->right = deallocateProcessMemoryBuddyRec(currNode->right, id, mid, right);

        if (currNode->left == NULL && currNode->right == NULL && currNode->data->id == -1)
        {
            free(currNode);
            currNode = NULL;
            return NULL;
        }
        return currNode;
    }
}

struct TreeNode * deallocateProcessMemoryBuddy(struct ProcessStruct *process){
    return deallocateProcessMemoryBuddyRec(buddyMemoryNode, process->id, 0, MEMORY_SIZE);
}
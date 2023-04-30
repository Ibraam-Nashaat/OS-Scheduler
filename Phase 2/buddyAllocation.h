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
        return 0;
    }
    
    if((currNode->data != NULL && currNode->data->id != -1)) {
        return 0;
    }

    if(currNode->size / 2 >= p->memSize) {
        int mid = (right + left) / 2;

        // try left
        if(currNode->left == NULL) {
            currNode->left = createTreeNode(currNode->size / 2);
        }

        int try_left = allocateProcessMemoryBuddyRec(currNode->left, p, left, mid);

        if(try_left == 1) {
            currNode->data = (struct ProcessStruct *)malloc(sizeof(struct ProcessStruct));
            currNode->data->id = -1;
            return 1;
        }

        if(currNode->right == NULL) {
            currNode->right = createTreeNode(currNode->size / 2);
        }

        int try_right = allocateProcessMemoryBuddyRec(currNode->right, p, mid, right);
        
        if(try_right == 1) {
            if(currNode->data != NULL) {
                currNode->data = (struct ProcessStruct *)malloc(sizeof(struct ProcessStruct));
                currNode->data->id = -1;
            }
            return 1;
        }
        return 0;
    } else if(currNode->data == NULL) {
        currNode->data = p;
        printf("the process %d settled from %d to %d size %d\n", p->id, left, right - 1, currNode->size);
        return 1;
    } else {
        printf("error no space in the TreeNode %d %d\n", p->id, currNode->size);
        return 0;
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
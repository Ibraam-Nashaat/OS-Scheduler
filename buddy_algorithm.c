#include<stdio.h>
#include<stdlib.h>


struct process{
    int id,
    size;
};

struct node
{
    struct process* data;
    struct node* left, *right;
    int size;
    int splitted;
};

struct node* make_new_node(int size)
{
    struct node* n = (struct node *)malloc(sizeof(struct node));
    n->data = NULL;
    n->left = NULL;
    n->right = NULL;
    n->size = size;
    int splitted = 0;
    return n;
}

struct node* memory; 


int add_to_memory(struct node* currNode ,struct process* p, int left, int right){
    
    if(p->size > currNode->size){
        return -1;
    }
    
    if((currNode->data != NULL && currNode->data->id != -1))
        return -1;

    if( currNode->size / 2 >= p->size)
    {
        int mid = (right + left) / 2;

        // try left
        if(currNode->left == NULL)
            currNode->left = make_new_node(currNode->size / 2);
        //printf("the process %d went left %d\n",p->id , currNode->size);
        
        int try_left = -1;
        //if(currNode->left->data == NULL)
        try_left =  add_to_memory(currNode->left, p, left, mid); 
        
        if(try_left == 1)
            {
                currNode->data = (struct process *) malloc(sizeof(struct process));
                currNode->data->id = -1;
                return 1;
            }
        //printf("fault in left %d\n", currNode->size);
        // try right
        
        int try_right = -1;
        if(currNode->right == NULL)
            currNode->right = make_new_node(currNode->size / 2);
        //printf("the process %d went right %d\n",p->id ,currNode->size);
        
        //if(currNode->right->data == NULL)
            try_right =  add_to_memory(currNode->right, p, mid, right);
        if(try_right == 1)
            {
                if(currNode->data != NULL)
                    {
                        currNode->data = (struct process *) malloc(sizeof(struct process));
                        currNode->data->id = -1;
                    }
                return 1;
            }    
        // return -1
        //printf("fault in right %d\n", currNode->size);    

        //printf("error no space in the node %d %d\n",p->id, currNode->size);
        return -1;
    }
    else if(currNode->data == NULL)
    {
        currNode->data = p;
        printf("the process %d settled from %d to %d size %d\n", p->id, left, right - 1, currNode->size);
        return 1; 
    }
    else
    {
        printf("error no space in the node %d %d\n",p->id, currNode->size);
        return -1;
    }
}

struct node* delete_from_memory(struct node* currNode, int id, int left, int right)
{
    if(currNode == NULL)
        return NULL;

    int mid = (right + left) / 2;

    if(currNode->data != NULL && currNode->data->id == id)
    {   
        printf("the process %d removed from %d to %d size %d\n", currNode->data->id, left, right - 1, currNode->size);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
        return NULL;
    }
    else 
    {
        currNode->left = delete_from_memory(currNode->left, id, left, mid);
        currNode->right = delete_from_memory(currNode->right, id, mid, right);

        if(currNode->left == NULL && currNode->right == NULL && currNode->data->id == -1)   
            {
                free(currNode);
                currNode = NULL;
                return NULL;
            }
        return currNode;    
    }    
        
}


int main()
{
    memory = make_new_node(1024);

    struct process * p1 = (struct process*) malloc(sizeof(struct process));
    p1->id = 1;
    p1->size = 400;
    add_to_memory(memory,p1 ,0, 1024);

    struct process * p2 = (struct process*) malloc(sizeof(struct process));
    p2->id = 2;
    p2->size = 100;
    add_to_memory(memory,p2 ,0, 1024);

    struct process * p3 = (struct process*) malloc(sizeof(struct process));
    p3->id = 250;
    p3->size = 250;
    add_to_memory(memory,p3, 0, 1024);

    struct process * p4 = (struct process*) malloc(sizeof(struct process));
    p4->id = 4;
    p4->size = 425;
    add_to_memory(memory, p4, 0, 1024);

    struct process * p5 = (struct process*) malloc(sizeof(struct process));
    p5->id = 5;
    p5->size = 100;
    add_to_memory(memory, p5, 0, 1024);

    // struct process * p6 = (struct process*) malloc(sizeof(struct process));
    // p5->id = 6;
    // p5->size = 100;
    // add_to_memory(memory, p6, 0, 1024);

    delete_from_memory(memory, p1->id, 0, 1024);
    delete_from_memory(memory, 2, 0, 1024);

    // p1 = (struct process*) malloc(sizeof(struct process));
    // p1->id = 1;
    // p1->size = 200;
    // add_to_memory(memory,p1 ,0, 1024);

    // p2 = (struct process*) malloc(sizeof(struct process));
    // p2->id = 2;
    // p2->size = 200;
    // add_to_memory(memory,p2 ,0, 1024);

    return 0;
} 
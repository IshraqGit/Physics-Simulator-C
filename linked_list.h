int keyCursor = 0;

typedef struct Node {
    int key;
    void* value;
    struct Node* next;
} Node;

void insertNode(Node** node, void* value) {
    Node* tmp = *node;
    Node* newNode = (Node*)malloc(sizeof (struct Node));
    newNode->key = keyCursor++;
    newNode->value = value;
    newNode->next = NULL;
    if(*node == NULL) {
        *node = newNode;
        return;
    }
    while(tmp->next)
        tmp = tmp->next;
    tmp->next = newNode;
}

void deleteAllFromCurrent(Node** node) {
    Node* tmp = *node;
    *node = NULL;

    if((tmp)->next) {
        deleteAllFromCurrent(&((tmp)->next));
        free(tmp);
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Node {
  char data;
  unsigned int freq;
  struct Node *left;
  struct Node *right;
} Node;

typedef struct Tree {
  unsigned int size;
  unsigned int capacity;
  struct Node* root;
} Tree;

typedef struct Queue {
  struct Node** array;
  int size;
  int capacity; // max size
} Queue;


/*                         *
 * simple helper functions *
 *                         */

// is the queue full
int isQueueFull(Queue *q) {
  return q->size == q->capacity;
}

// is the node a leaf node, i.e. does the node contain the symbol
int isLeafNode (Node *node) {
  return (node->left == NULL && node->right == NULL);
}

// create new node
Node* newNode(char data, unsigned int freq) {
  Node* new = malloc(sizeof(Node));

  new->data = data;
  new->freq = freq;
  new->left = NULL;
  new->right = NULL;

  return new;
}

Tree* createTree(unsigned int capacity) {
  struct Tree* newTree = malloc(sizeof(Tree));
  newTree->capacity = capacity;
  newTree->size = 0;

  newTree->root = NULL;

  return newTree;
}

// Less than zero (<0): If the first argument should be placed before the second argument.
// Zero (0): If both arguments are equal.
// Greater than zero (>0): If the first argument should be placed after the second argument.
int compare_freq(const void *a, const void *b) {
  Node *nodeA = *(Node **)a; // cast and dereference to get the Node pointer
  Node *nodeB = *(Node **)b;

  // first sort by freq
  if (nodeA->freq > nodeB->freq)
      return 1; // higher frequency comes after lower frequency
  else if (nodeA->freq < nodeB->freq)
      return -1; // lower frequency comes before higher frequency

  // sort by data alphabetically
  if (nodeA->data > nodeB->data)
    return 1;
  else if (nodeA->data < nodeB->data)
    return -1;

  return 0; // equal nodes
}

Queue createQueue(char data[], unsigned int freq[], int size) {
  Queue q;
  Node **arr = malloc(sizeof(Node*) * size);
  for (int i = 0; i < size; i++) {
    arr[i] = malloc(sizeof(Node));
    arr[i]->data = data[i];
    arr[i]->freq = freq[i];
    arr[i]->left = NULL;
    arr[i]->right = NULL;
  }

  q.size = size;
  q.capacity = size;
  q.array = arr;

  // sort the array in ascending order
  qsort(q.array, size, sizeof(Node *), compare_freq);

  return q;
}

void fix_size(Tree *T)
{
  T->size = 1;
  if (T->root->left)
    T->size++;
  if (T->root->right)
    T->size++;
}

// insert key k into tree T, returning a pointer to the resulting root
Node *insert(Node *T, char data, unsigned int freq) {
  if (T == NULL) return newNode(data, freq);
  if (freq < T->freq)
    T->left = insert(T->left, data, freq);
  else
    T->right = insert(T->right, data, freq);
  return T;
}

// join trees L and R (with L containing keys all <= the keys in R)
// args: two pointers to Nodes of subtrees
// returns: a pointer to the joined tree.
Node *join(Node *L, Node *R)
{
  // |L|/(|L|+|R|) & |R|/(|L|+|R|)
  //Implement Node *join(Node *L, Node *R)
  if (L == NULL)
    return R;
  if (R == NULL)
    return L;
  Node *newRoot;
  // right root becomes root
    if (R->left == NULL)
      R->left = L;
    else R->left = join(L, R->left);
    newRoot = R;
  return newRoot;
}

// sorts passed data in ascending order
// returns pointer to node array
Node **sort_ascending(Queue q, int size) {
  qsort(q.array, size, sizeof(Node *), compare_freq);
  return q.array;
}

// gets and removes the front from the queue
Node *dequeue(Queue *q) {
  if (q->size == 0) {
    printf("Queue is empty\n");
    return NULL;
  }

  Node *min = q->array[0];

  for (int i = 1; i < q->size; i++) {
    q->array[i-1] = q->array[i];
  }

  q->size--;

  // printf("dequeued: %d:%c\n", min->freq, min->data);

  // re-sort
  qsort(q->array, q->size, sizeof(Node *), compare_freq);

  return min;
}

// function to enqueue to the back of the queue
void insertQueue(Queue *q, Node* node) {
  if (isQueueFull(q)) return;

  q->array[q->size] = node;
  q->size++;


  // Re-sort the queue

  // printf("after inserting: %d:%c\n", node->freq, node->data);
  // for (int i = 0; i < q->size; i++) {
  //   printf("%d:%c\n", q->array[i]->freq, q->array[i]->data);
  // }
}

// creates a queue that is sorted in ascending order
// returns a pointer to the root of the huffman tree that is created
Node *buildTree(char data[], int freq[], int size) {
  Node *left, *right, *top;

  // array of parent nodes
  Queue parent_array;
  parent_array = createQueue(data, freq, size);

  // printf("initialized queue: \n");
  // for(int i = 0; i < size; i++)
  //   printf("%d:%c\n", parent_array.array[i]->freq, parent_array.array[i]->data);

  // Iterate while size of queue doesn't become 1
  while (parent_array.size != 1) {

      // Step 2: Extract the two minimum freq items from the queue
      left = dequeue(&parent_array);
      right = dequeue(&parent_array);
      // printf("combining two smallest nodes: %d:%c   %d:%c\n", left->freq, left->data, right->freq, right->data);

      top = newNode('\0', left->freq + right->freq);

      top->left = left;
      top->right = right;

      insertQueue(&parent_array, top);
      qsort(parent_array.array, parent_array.size, sizeof(Node *), compare_freq);

      // printf("size of Queue: %d\n\n", parent_array.size);

  }

  // the final remaining node is the root node and the tree is complete
  return dequeue(&parent_array);
}

void encode(Node *root, int arr[], int top) {
  if (root->left) {
    arr[top] = 0;
    encode(root->left, arr, top + 1);
  }

  if (root->right) {
    arr[top] = 1;
    encode(root->right, arr, top + 1);
  }

  // if node is a leaf node then print enpty the buffered code into the file
  if (isLeafNode(root)) {
    printf("%c: ", root->data);
    for(int i = 0; i < top; i++) printf("%d", arr[i]);
    printf("\n");
  }
}

// function to perform inorder traversal and print
void print_inorder(Node *node) {
  if (node == NULL) {
    return;
  }

  // Traverse left subtree
  print_inorder(node->left);

  // Visit root (current node)
  printf("%d:%c ", node->freq, node->data);

  // Traverse right subtree
  print_inorder(node->right);
}

// work in progress to print the tree in tree format
void printLevelOrder(Node *root, int size) {
  if (root == NULL) return;

  // Queue to store nodes for level order traversal
  Queue q;
  Node **arr = malloc(sizeof(Node*) * size);
  for (int i = 0; i < size; i++) {
    arr[i] = malloc(sizeof(Node));
  }

  q.size = 0;
  q.capacity = size;
  q.array = arr;

  // Enqueue the root node
  insertQueue(&q, root);
  printf("\n");
  while (q.size != 0) {
    // Get the current level size
    int levelSize = q.size;
    // Process all nodes at the current level
    for (int i = 0; i < levelSize; ++i) {
      Node *node = dequeue(&q);
      printf("%d:%c    ", node->freq, node->data);

      // Enqueue left child if it exists
      if (node->left != NULL)
        insertQueue(&q, node->left);

      // Enqueue right child if it exists
      if (node->right != NULL)
        insertQueue(&q, node->right);
    }
    // Print new line after each level is processed
    printf("\n");
  }
}

int main(int argc, char *argv[]){
  if (argc != 4) {
    printf("Usage: %s <input file> <encoded file> <decoded file>\n", argv[0]);
    return 1;
  }

  int freq[256] = {0};
  FILE* input = fopen(argv[1], "r");
  if (!input) {
    printf("Error opening input file\n");
    return 1;
  }
  // Read entire input file
  char ch;
  while ((ch = fgetc(input)) != EOF){
    freq[(unsigned char)ch]++;
  }

  fclose(input);

  // Extracts data and stores their corresponding frequency
  char data[256];
  int freqArray[256], size = 0;
  for (int i = 0; i < 256; i++) {
    if (freq[i] > 0) {
      data[size] = (char)i;
      freqArray[size] = freq[i];
      size++;
    }
  }

  Tree *tree = createTree(sizeof(data)/sizeof(data[0]));
  printf("%d\n",size);
  tree->root = buildTree(data, freqArray, size);
  print_inorder(tree->root);
  printf("\n\n");
  printf("%d:%c\n", tree->root->freq, tree->root->data);

  int arr[100], top = 0;
  encode(tree->root, arr, top);

  return 0;
}

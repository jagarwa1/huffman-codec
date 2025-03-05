/*
 * Jai Agarawl & Darron Yi
 * ECE 4680 - Huffman Codec
 * 
 * This file implements Huffman encoding and decoding.
 *
 * compile with: 'gcc lab4.c -o huffman'
 * run with:     './huffman -c [file]' to create [file].huf    (compressed)
 *               './huffman -d [file].huf' to create [file].d  (decompressed)
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

// sorts passed data in ascending order
// returns pointer to node array
Node **sort_ascending(Queue q, int size) {
  qsort(q.array, size, sizeof(Node *), compare_freq);
  return q.array;
}

void print_inorder(Node *node);

void printLevelOrder(Node *root, int size);

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
  Tree* newTree = malloc(sizeof(Tree));
  newTree->capacity = capacity;
  newTree->size = 0;

  newTree->root = NULL;

  return newTree;
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

  return min;
}

// function to enqueue to the back of the queue
void insertQueue(Queue *q, Node* node) {
  if (isQueueFull(q)) return;

  q->array[q->size] = node;
  q->size++;

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

// function to encode the tree into array codes
void encode(Node *root, char arr[], int top, char codes[256][100]) {
  if (root->left) {
    arr[top] = '0';
    encode(root->left, arr, top + 1, codes);
  }

  if (root->right) {
    arr[top] = '1';
    encode(root->right, arr, top + 1, codes);
  }

  // if node is a leaf node then print the buffered code into the file
  if (isLeafNode(root)) {
    arr[top] = '\0';
    strcpy(codes[(unsigned char)root->data], arr); // copy generated code into 'codes' array
  }
}

void writeHeaderToFile(char *outputFile, char data[], int freq[], int size) {
  FILE* output = fopen(outputFile, "wb");
  char str[50];

  // write size of the table first, with a space
  sprintf(str, "%d " , size);
  fwrite(str, 1, strlen(str), output);
  
  // write elements of the table
  // writes int followed by char
  for(int i = 0; i < size; i++) {
    sprintf(str, "%d:%c", freq[i], data[i]);
    fwrite(str, 1, strlen(str), output);
  }
  fclose(output);
}

// parses header of a compressed file, create tree based on freq data read in
// return root node of tree
long parseHeaderBuildTree(char *inputFile, Node **root) {
  FILE* input = fopen(inputFile, "r");
  if (!input) {
    printf("Error opening input file\n");
    return -1;
  }

  char in;
  int size = 0;
  long position; // record file offset;

  // get size of table
  char terminator = ' ';
  char buffer[128];
  int index = 0;
  while ((in = fgetc(input)) != EOF) { // Read character by character
    if (in == terminator) {
      break;  // Stop when the terminator is found
    }
    buffer[index++] = in;
    buffer[index] = '\0';  // Null-terminate each step
  }
  size = atoi(buffer);

  int freq[size];
  char data[size];

  terminator = ':';
  // read size num of char and int pairs
  // read int(fre1) followed by char(data)
  for (int i = 0; i < size; i++) {
    index = 0;
    while ((in = fgetc(input)) != EOF) { // Read character by character
      if (in == terminator) {
        break;  // Stop when the terminator is found
      }
      buffer[index++] = in;
      buffer[index] = '\0';  // Null-terminate each step
    }
    freq[i] = atoi(buffer);
    fread(&data[i], sizeof(char), 1, input);
  }
  position = ftell(input);
  fclose(input);

  *root = buildTree(data, freq, size);
  printf("tree of size: %d\n", size);
  printLevelOrder(*root, size);

  return position;
}

// function to take the codes and insert them into an output file
void compress(const char *inputFile, const char *outputFile, char codes[256][100]){
  FILE* input = fopen(inputFile, "rb");
  FILE* output = fopen(outputFile, "ab");
  unsigned char buffer = 0;
  int bitCount = 0;
  char ch;
  while ((ch = fgetc(input)) != EOF) {
    char* code = codes[(unsigned char)ch];
    for (int i = 0; code[i] != '\0'; i++) {
      buffer = (buffer << 1) | (code[i] - '0');
      bitCount++;
      if (bitCount == 8) {
        fwrite(&buffer, 1, 1, output);
        buffer = 0;
        bitCount = 0;
      }
    }
  }
  if (bitCount > 0) {
    buffer <<= (8 - bitCount);
    fwrite(&buffer, 1, 1, output);
  }
  fclose(input);
  fclose(output);
}

// function to take the tree root, read from a file, and decode using the tree
void decompress(Node* root, char* encodedFile, char* decodedFile, long file_offset) {
  FILE* input = fopen(encodedFile, "rb");
  FILE* output = fopen(decodedFile, "wb");
  Node* curr = root;
  unsigned char buffer;

  fseek(input, file_offset, SEEK_SET); // seek to after the header
  while (fread(&buffer, 1, 1, input)) {
    for (int i = 7; i >= 0; i--) {
      curr = ((buffer >> i) & 1) ? curr->right : curr->left;
      if (!curr->left && !curr->right) {
        fputc(curr->data, output);
        curr = root;
      }
    }
  }
  fclose(input);
  fclose(output);
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

  int tier = 0;
  // Enqueue the root node
  insertQueue(&q, root);
  printf("\n");
  while (q.size != 0) {
    // pad with spaces based on what tier this is
    for (int i = 0; i < 10 - tier; i++) printf("     ");
  
    // Get the current level size
    int levelSize = q.size;
    // Process all nodes at the current level
    for (int i = 0; i < levelSize; i++) {
      Node *node = dequeue(&q);
      printf("%d:%c", node->freq, node->data);  
      for (int j = 0; j < 7 - levelSize; j++) printf(" ");
      
      // Enqueue left child if it exists
      if (node->left != NULL)
        insertQueue(&q, node->left);

      // Enqueue right child if it exists
      if (node->right != NULL)
        insertQueue(&q, node->right);
    }
    // Print new line after each level is processed
    printf("\n");
    tier++;
  }
}

int main(int argc, char *argv[]){
  if (argc != 3) {
    printf("Usage: %s -c|-d <input file>\n", argv[0]);
    return 1;
  }

  int freq[256] = {0};
  char *inputFile = argv[2];
  char *outputFile = (char *)malloc(sizeof(argv[2] + 6));
  
  Tree *tree;
  FILE* input;

  /****************** encode *****************/
  if (strcmp(argv[1], "-c") == 0) {
    // setting outputFile
    strcpy(outputFile, inputFile);
    strcat(outputFile, ".huff");

    input = fopen(inputFile, "rb");
    if (!input) {
      printf("Error opening input file\n");
      return 1;
    }
  
    // read entire input file one byte ate a time, generating freqs
    char ch;
    while ((ch = fgetc(input)) != EOF){
      freq[(unsigned char)ch]++;
    }

    fclose(input);

    // extracts data and stores their corresponding frequency
    char data[256];
    int freqArray[256], size = 0;
    for (int i = 0; i < 256; i++) {
      if (freq[i] > 0) {
        data[size] = (char)i;
        freqArray[size] = freq[i];
        size++;
      }
    }

    // create and build the tree
    tree = createTree(size);
    tree->root = buildTree(data, freqArray, size);

    /* printing out */
    printf("tree of size: %d\n",size);
    print_inorder(tree->root);
    printf("\n\n");
    printf("%d:%c\n", tree->root->freq, tree->root->data);
    printLevelOrder(tree->root, tree->size);
    /* end printing out */

    // generate codes from tree
    char arr[100], top = 0;
    char codes[256][100] = {0};
    encode(tree->root, arr, top, codes);
    
    // write header to file and then write compressed data
    writeHeaderToFile(outputFile, data, freqArray, size);
    compress(inputFile, outputFile, codes);
  }
  /********************* decode *********************/
  else if (strcmp(argv[1], "-d") == 0) { 
    // setting outputFile
    strncpy(outputFile, inputFile, strlen(inputFile)-5);
    strcat(outputFile, ".dec");

    // parse the header to get freqs and create the tree
    Node *root = NULL;
    long file_offset = parseHeaderBuildTree(inputFile, &root);
    if (file_offset < 0) {
      printf("error reading header of file\n");
      return 1;
    }
        
    // decompress and write to file
    decompress(root, inputFile, outputFile, file_offset);

  }
  return 0;
}

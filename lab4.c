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

void fix_size(Tree *T)
{
  T->size = 1;
  if (T->root->left) 
    T->size++;
  if (T->root->right) 
    T->size++;
}

// insert key k into tree T, returning a pointer to the resulting root
Node *insert(Node *T, char data, unsigned int freq)
{
  if (T == NULL) return newNode(data, freq);
  if (freq < T->freq) 
    T->left = insert(T->left, data, freq);
  else 
    T->right = insert(T->right, data, freq);
  return T;
}
// combine(){}

// Helper function to perform inorder traversal and
// populate the 2D matrix
void inorder(Node *root, int row, int col, int height) {
    if (!root) {
        return;
    }

    // Calculate offset for child positions
    int offset = pow(2, height - row - 1);

    // Traverse the left subtree
    if (root->left) {
        inorder(root->left, row + 1, col - offset, height);
    }

    printf("%d%c", root->freq, root->data);

    // Traverse the right subtree
    if (root->right) {
        inorder(root->right, row + 1, col + offset, height);
    }
}

void print_node(Node *node) {
  printf("%d:%c ", node->freq, node->data);
}
void print_tree(Node *node) {
  Node *root;

  printf("%d:%c\n", node->freq, node->data);
        

  if (node->left) {
    print_tree(node);
  }
  if (node->right) {
    print_tree(node);
  }
}

int main(int argc, char *argv[]){
  // if(argc != 4){
  //   fprintf(stderr, "Usage: %s compress|decompress inputfile outputfile\n", argv[0]);
  //   exit(1);
  // }
  // if(strcmp(argv[1], "-c") == 0){
  //   compress(argv[2], argv[3]);
  // }
  // else if(strcmp(argv[1], "-d") == 0){
  //   decompress(argv[2], argv[3]);
  // }
  // else{
  //   fprintf(stderr, "Invalid command. Use 'compress' or 'decompress'.\n");
  //   return exit(1);
  // }

  struct Tree *tree = createTree(5);

  tree->root = insert(tree->root, 'A', 3);
  tree->size++;

  tree->root = insert(tree->root, 'B', 5);
  tree->size++;

  tree->root = insert(tree->root, 'C', 1);
  tree->size++;

  print_tree(tree->root);

  return 0;
}

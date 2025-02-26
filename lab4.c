#include <stdio.h>

int main(int argv, char *argc[]){
  if(argc != 4){
    fprintf(stderr, "Usage: %s compress|decompress inputfile outputfile\n", argv[0]);
    return EXIT_FAILURE;
  }
  if(strcmp(argv[1], "c") == 0){
    compress(argv[2], argv[3]);
  }
  else if(strcmp(argv[1], "d") == 0){
    decompress(argv[2], argv[3]);
  }
  else{
    fprintf(stderr, "Invalid command. Use 'compress' or 'decompress'.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stddef.h>
#include "read_file.h"
int input_file_open(InputFile *input, const char *filename)
{
    input->file = fopen(filename, "r");

    if (input->file == NULL) {
        perror("Failed to open input file");
        return -1;
    }

    return 0;
}
int input_file_open_write(InputFile *file , const char *filename){
  file->file=fopen(filename,"w");

  if(file->file==NULL){
    perror("Failed to write to the file \n");
    return -1;
  }
  return 0;
}

int input_file_write(InputFile *file,float *x , float*y , float *error , int *status){
  fprintf(file->file,"%.2f %.2f %.2f %d \n",*x,*y,*error,*status);
  return 0;

}

int input_file_read(InputFile *input, float *x, float *y)
{
    return fscanf(input->file, "%f %f", x, y) == 2;
}

void input_file_close(InputFile *input)
{
    if (input->file != NULL) {
        fclose(input->file);
        input->file = NULL;
    }
}

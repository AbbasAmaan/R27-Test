#ifndef FILE_H
#define FILE_H 
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  FILE *file;
}InputFile;

typedef struct {
  int id;
  const char* filename;
  const char* result_filename;
}FileArgs;


int input_file_open(InputFile *input, const char *filename);
int input_file_open_write(InputFile *file , const char *filename);
int input_file_read(InputFile *input, float *x, float *y);

int input_file_write(InputFile *input , float *x , float *y,float *err , int *status);

void input_file_close(InputFile *input);


#endif

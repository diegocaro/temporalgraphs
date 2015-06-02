#ifndef _ARRAYSORT_H_
#define _ARRAYSORT_H_

void print_arraysort(unsigned int *a);
void diff_arraysort( unsigned int *a, const unsigned int *b);

int compare (const void * a, const void * b);
void remove_duplicates(unsigned int *b);

void merge_arraysort( unsigned int *res,  const unsigned int *a, const unsigned int *b);
#endif
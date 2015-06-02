/*
 * arraysort.c
 *
 *  Created on: Mar 22, 2013
 *      Author: diegocaro
 */

#include <stdio.h>
#include <sys/types.h>


void print_arraysort(unsigned int *a) {
  uint i;

  for (i = 1; i <= *a; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");

}

void diff_arraysort( unsigned int *a, const unsigned int *b) {
  uint i, j, u;
  i = j = u = 1;
  while(1) {
    if (i == a[0]+1) {
      break;
    }

    if (j == b[0]+1) {
      a[u] = a[i];
      u++; i++;
      continue;
    }


    if (a[i] == b[j]) {
      i++; j++;
      continue;
    }

    if (a[i] < b[j]) {
      a[u] = a[i];
      u++; i++;
      continue;
    }

    if (a[i] > b[j]) {
      j++;
      continue;
    }


  }

  *a = u-1;
}

int compare (const void * a, const void * b) {
  return ( *(unsigned int*)a - *(unsigned int*)b );
}



// b[] must be sorted
void remove_duplicates(unsigned int *b) {
  uint i, j;


  //doing the no duplicates operation.. for example 2 2 2 3 4 5 6 6 7 -> 2 3 4 5 6 7
  i = 0;
  for(j = 2; j<=*b; j++) {
    if ( b[j-1] == b[j]) continue; // se salta el numero
    else {
      b[++i] = b[j-1];
    }
  }
  //printf("b[%d] = %d\n", i,b[i]);
  if (j == *b + 1) {
    b[++i] = b[*b];
  }
  *b = i;

}


void merge_arraysort( unsigned int *res,  const unsigned int *a, const unsigned int *b) {
  uint i, j, u;

  i = j = u = 1;

  while (1) {

    if ( i == a[0]+1 && j == b[0]+1 ) {
      break;
    }

    if (i == a[0]+1) {
      res[u] = b[j];
      j++; u++;
      continue;
    }

    if (j == b[0]+1) {
      res[u] = a[i];
      u++; i++;
      continue;
    }

    if ( a[i] < b[j] ) {
      res[u] = a[i];
      i++; u++;
      continue;
    }

    if ( a[i] > b[j] ) {
      res[u] = b[j];
      j++; u++;
      continue;
    }

    if ( a[i] == b[j] ) {
      res[u] = a[i];
      i++; j++; u++;
      continue;
    }

  }

  *res = u-1;
}


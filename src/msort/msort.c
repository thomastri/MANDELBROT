/* Unithreaded merge sort implementation */

#include <stdio.h>              /* standard I/O */
#include <stdbool.h>            /* C99 boolean types */
#include <stdint.h>             /* C99 integer types */
#include <string.h>             /* declares memcpy() */
#include <inttypes.h>
#include <stdlib.h>

#include "msort.h"              /* merge sort interface header */

void merge_sort(index_value array[], uint32_t start, uint32_t end,
                bool (*less)(index_value, index_value), uint32_t bsort);

void merge(index_value array[], uint32_t start, uint32_t end,
           bool (*less)(index_value, index_value));

void bubble(index_value array[], uint32_t start, uint32_t end,
            bool (*less)(index_value, index_value));

static char **parse_file(char *file, char **p);

index_value *make_struct(char *file, index_value *array);

/* entry point to merge sort. Do not change the interface */
int
run_msort(char *params, index_value **_array) {
    char *arr[5];
    ms_args sort_struct;
    char* filename = NULL;
    int k;

    


    parse_file(params, arr);

    filename = arr[1];
    index_value *array = malloc(sizeof (index_value) * (atoi(arr[2])));

    
    sort_struct.start = 0;
    sort_struct.array = make_struct(filename, array);
    sort_struct.end = atoi(arr[2])-1;
    sort_struct.bubble_threshold = atoi(arr[4]);
    sort_struct.less = &comp_coords;
    sort_struct.parallel_depth = atoi(arr[3])-1;

    merge_sort(sort_struct.array, sort_struct.start, sort_struct.end,sort_struct.less,sort_struct.bubble_threshold);
    

    FILE *fp = fopen(arr[0], "w");
    if (fp == NULL)
        return 0;
    for(k=0; k < sort_struct.end+1; k++)
    {
       fprintf(fp,"%d,%d,%f\n", sort_struct.array[k].i, sort_struct.array[k].j, sort_struct.array[k].value);
    }
    fclose(fp);
    
    free(array);


    return 0;
}




static char **
parse_file(char *file, char **p) {


    FILE *fp = fopen(file, "r");
    char *token;
    char line[200];
    int counter = 0;

    if (fp != NULL) 
    {


        while (fgets(line, 200, fp) != NULL) {
            token = strtok(line, ",");

            while (token != NULL) 
	    {
		p[counter] = strdup(token);
                token = strtok(NULL, ",");

                counter++;
            }
	    
        }
        
    } 
    else {
        perror("File not read");
    }
    fclose(fp);

    return p;
    
}

index_value*
make_struct(char *file, index_value *array) {

    FILE *fp = fopen(file, "r");
    char *token;
    char line[200];
    int counter = 0;

    //read the file if present
    if (fp != NULL) 
    {
        
        while (fgets(line, 200, fp) != NULL) 
	{
	    //printf("line: %s\n", line);
            token = strtok(line, ",");	
	    array[counter].i = atoi(token);
	    //printf("token1: %d\n", array[counter].i);

            token = strtok(NULL, ",");	
	    array[counter].j = atoi(token);

            token = strtok(NULL, ",");

	    array[counter].value = atof(token);
	    
	    counter++;
        }
	    
    } 
    else {
        perror("File not read");
    }
    fclose(fp);



    return array;
    

}
/* unithreaded merge sort implementation */
void
merge_sort(index_value array[], uint32_t start, uint32_t end,
           bool (*less)(index_value, index_value), uint32_t bsort) {
    /* calculate the index in the middle of the array */
    uint32_t middle = (end + start) / 2;
    uint32_t right_start = middle + 1;

    //thread max counter

    /* if the size has fallen below the threshold, switch to bubble sort */
    if ((end - start) < bsort) {
        bubble(array, start, end, less);
        return;
    }



    /* split the array into a left and right half, and sort both */
    merge_sort(array, start, middle, less, bsort);
    merge_sort(array, right_start, end, less, bsort);

    /* merge the sorted arrays */
    merge(array, start, end, less);
}

#define MAX_TEMP_ARRAY 128

/* routine to merge two sorted arrays. You should not change this. */
void
merge(index_value array[], uint32_t start, uint32_t end,
      bool (*less)(index_value, index_value)) {
    index_value temp[MAX_TEMP_ARRAY];
    uint32_t temp_index, i;
    uint32_t right_index = ((end + start) / 2) + 1;
    uint32_t left_index = start;

    /* start at the beginning of the left and right arrays */
    while (right_index <= end && left_index < right_index) {
        /* if array[right] < array[left], swap them */
        if (less(array[right_index], array[left_index])) {
            /* Get several items from the right that are bigger than left. This
             * provides a minor performance improvement. */
            i = 0;
            do {
                i++;
            } while (less(array[right_index + i], array[left_index]) &&
                     i < MAX_TEMP_ARRAY && (right_index + i) <= end);

            /* copy up to 128 items from the right into a temp array */
            memcpy(temp, &array[right_index], i * sizeof(index_value));

            /* shift the left array to the right */
            for (temp_index = right_index; temp_index > left_index;
                 temp_index--)
                array[temp_index + i - 1] = array[temp_index - 1];

            /* now copy the temp items into the left */
            memcpy(&array[left_index], temp, i * sizeof(index_value));
            right_index += i;
        }
        left_index++;
    }
   
    

}

#undef MAX_TEMP_ARRAY

/* bubble sort implementation. This is just basic bubble sort and shouldn't
 * need modified. */
void
bubble(index_value array[], uint32_t start, uint32_t end,
       bool (*less)(index_value, index_value)) {
    uint32_t i, j;
    index_value temp;

    for (i = end; i > start; i--) {
        for (j = start; j < i; j++) {
            if (less(array[j + 1], array[j])) {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

/* comparison function to compare the (i,j) coordinates of two pixels */
bool
comp_coords(index_value a, index_value b) {
    if (a.i < b.i) return true;
    if ((a.i == b.i) && (a.j < b.j)) return true;
    return false;
}


/* Merge sort implementation */
void
merge_sort_multi(index_value array[], uint32_t start, uint32_t end,
           bool (*less)(index_value, index_value), uint32_t bsort, uint8_t threads ) {
    /* calculate the index in the middle of the array */
    pthread_t threads[THREAD_LIMIT];
    uint32_t middle = (end + start) / 2;
    uint32_t right_start = middle + 1;
 
    //int thread_limit = //some param THREAD_LIMIT	//AA9/19/15
    //int thread_count = 0;				//AA9/19/15
    //int rc = 0;					//AA9/19/15

    if (thread_count < thread_limit) //AA9/19/15
    {
	printf ("starting msort thread %d\n", thread_count);        

	/* if the size has fallen below the threshold, switch to bubble sort */
        if ((end - start) < bsort) {
	  printf ("starting thread %d bubblesort\n", thread_count);
          bubble(array,start,end,less);
	  rc = pthread_create (&threads[thread_count], NULL, bubble, (void*)thread_count);
	  //thread_count++;
	  // bubble(array, start, end, less);
          return;
        }
	
	
		/* split the array into a left and right half, and sort both */
        if (thread_count + 1 < thread_limit)
        { 
        
          printf ("starting thread %d\n merge", thread_count);
		  rc = pthread_create (&threads[thread_count], NULL, merge_sort, (void*)thread_count);
		  //merge_sort(array, start, middle, less, bsort);
		  thread_count++;
        
         printf ("starting thread %d\n merge", thread_count);
 	     rc = pthread_create (&threads[thread_count], NULL, merge_sort, (void*)thread_count);
         //merge_sort(array, right_start, end, less, bsort);

		 thread_count++;
	}
	else if (thread_count < thread_limit)
	{
		printf ("starting thread %d\n merge", thread_count);
		rc = pthread_create (&threads[thread_count], NULL, merge_sort, (void*)thread_count);
		thread_count++;		
		
		merge_sort(array, right_start, end, less, bsort);
    }
    else
    {
    
        /* if the size has fallen below the threshold, switch to bubble sort */
        if ((end - start) < bsort) {
            bubble(array, start, end, less);
            return;
        }

        /* split the array into a left and right half, and sort both */
        merge_sort(array, start, middle, less, bsort);
        merge_sort(array, right_start, end, less, bsort);

        /* merge the sorted arrays */
        merge(array, start, end, less);
   }
}



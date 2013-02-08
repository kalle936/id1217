/**
 * Program that mimics UNIX command tee but working in parallel. 
 * 
 * Main thread initializes everything and then creates one thread to read from stdin,
 * one thread to write to stdout and one thread to write to file.
 *
 * Compile through make file, either by default rule or by the rule tee.out
 * Run through executing ./tee.out filename
 */


#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define BUFFERSIZE 10 /* Size of buffer. */
#define LINESIZE 256 /* Maximum size of the char[] */

char * buffer[BUFFERSIZE]; /* The buffer */
FILE * fp; /* Filepointer to the file. */

int read_index; /* Index to where next read is supposed to be inserted. */
int write_stdout_index; /* Next char[] to be written to stdout. */
int write_file_index; /* Next char[] to be written to the file. */

bool done; /* If EOF is set in stdin. */

pthread_mutex_t lock; /* Lock to make conditional variables work */
pthread_cond_t read_go; /* Used to signal to the reading thread to start reading again. */
pthread_cond_t write_go; /* Used to signal to the writing threads to start writing again. */



void * read(void *);
void * write_file(void *);
void * write_stdout(void *);

/**
 * Reads arguments and open the file then creates the three worker threads.
 * 
 * Because buffer is saved on heap the threads are joined and then the buffer is freed.
 */
int main(int argsc, char ** argsv)
{
    int i; /* Index. */
    if(argsc <= 1) /* If there was no argument, there was no filename and we exit. */
    {
        printf("Usage: ./tee.out filename\n");
        exit(0);
    }

    for(i = 0; i < BUFFERSIZE; i++) /* Allocates space of the buffer on the heap. */
    {
        buffer[i] = malloc(LINESIZE * sizeof(char));
    }

    /* Initializes the shared variables of the indices. */
    read_index = 0;
    write_stdout_index = 0;
    write_file_index = 0;

    done = false; /* Initialize done. */

    fp = fopen(argsv[1], "w"); /* Opens the file. */
    if(fp == NULL) /* Checks if somehting went wrong when opening the file. */
    {
        fprintf(stderr, "Could not open the file.\n");
        pthread_exit(0);
    }

    /* Initializing for using pthread.  */
    pthread_attr_t attr;
    pthread_t workerid[3];

    /* Set global thread attribute. */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* Initialize the lock and the cond. */
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&read_go, NULL);
    pthread_cond_init(&write_go, NULL);
    
    /* Creates the three workers with the different methods. */
    pthread_create(&workerid[0], &attr, read, NULL); 
    pthread_create(&workerid[1], &attr, write_file, NULL); 
    pthread_create(&workerid[2], &attr, write_stdout, NULL); 

    /* Joins them again. When this is done the sorting is complete. */
    for(i = 0; i < 3; i++)
    {
        pthread_join(workerid[i], NULL);
    }

    fclose(fp); /* Close the file */
    
    for(i = 0; i < BUFFERSIZE; i++) /* Frees the space that the buffer allocated on the heap. */
    {
        free(buffer[i]);
    }
    pthread_exit(NULL);
}

/**
 * Worker threads that reads from stdin and puts it in the shared buffer.
 */
void * read(void * arg)
{
    char * local_buffer = malloc(LINESIZE * sizeof(char)); /* Using a local buffer when reading input. */

    while(fgets(local_buffer, LINESIZE, stdin)) /* Keep reading until EOF is reached. */
    {
        /* Locks and checks if the local buffer can be copied over to the shared buffer. */
        pthread_mutex_lock(&lock);
        while((read_index + 1) % BUFFERSIZE == write_stdout_index || (read_index + 1) % BUFFERSIZE == write_file_index)
        {
            pthread_cond_wait(&read_go, &lock); /* We wait until the other two workers have written. */
        }
        pthread_mutex_unlock(&lock);

        memcpy(buffer[read_index], local_buffer, LINESIZE); /* Copies over the local buffer to the shared buffer. */
        
        /* Locks, updates read_index and then signal to the idle writing workers to start writing. */
        pthread_mutex_lock(&lock);
        read_index = (read_index + 1) % BUFFERSIZE;
        pthread_cond_broadcast(&write_go);
        pthread_mutex_unlock(&lock);

    }

    done = true; /* When EOF is reached we are done and this shared variable is used to signal to the writers to terminate. */

    pthread_cond_broadcast(&write_go); /* Wakes up the idle writers. */
    pthread_exit(NULL);
}

/**
 * Worker thread that writes to file from the shared buffer.
 */
void * write_file(void * arg)
{
    while(true)
    {
        /* Locks and checks if there is anything to write to file. */
        pthread_mutex_lock(&lock);
        if(read_index == write_file_index)
        {
            pthread_cond_wait(&write_go, &lock); /* Waits for anything to write to file. */
        }
        pthread_mutex_unlock(&lock);
        if(done) /* There is nothing more to coming from the reader thread. Write everything in the buffer. */
        {
            while(read_index != write_file_index)
            {
                fprintf(fp, "%.*s", LINESIZE, buffer[write_file_index]);
                write_file_index = (write_file_index + 1) % BUFFERSIZE;
            }
            break; /* When we written everything in the buffer we break the loop. */
        }
        fprintf(fp, "%.*s", LINESIZE, buffer[write_file_index]); /* Writes to the file. */

        /* Locks, updates write_file_index and then signal to idle read thread. */
        pthread_mutex_lock(&lock);
        write_file_index = (write_file_index + 1) % BUFFERSIZE;
        pthread_cond_broadcast(&read_go);
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

/**
 * Worker thread that writes to stdout from the shared buffer.
 */
void * write_stdout(void * arg)
{
    while(true)
    {
        /* Locks and checks if there is anything to write to file. */
        pthread_mutex_lock(&lock);
        if(read_index == write_stdout_index)
        {
            pthread_cond_wait(&write_go, &lock); /* Waits for anything to write to stdout. */
        }
        pthread_mutex_unlock(&lock);
        if(done) /* There is nothing else coming from the reader. Write everything remaining in the buffer to stdout. */
        {
            while(read_index != write_stdout_index)
            {
                printf("%.*s", LINESIZE, buffer[write_stdout_index]);
                write_stdout_index = (write_stdout_index + 1) % BUFFERSIZE;
            }
            break; /* Breaks the loop when we are done writing to stdout. */
        }
        printf("%.*s", LINESIZE, buffer[write_stdout_index]); /* Write to stdout. */

        /* Locks, updates write_stdout_index and then signal to the idle read thread. */
        pthread_mutex_lock(&lock);
        write_stdout_index = (write_stdout_index + 1) % BUFFERSIZE;
        pthread_cond_broadcast(&read_go);
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

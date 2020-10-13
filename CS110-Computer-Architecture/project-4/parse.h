#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>

#ifndef __APPLE__ // for MAC_OSX do not have this file
#include <linux/limits.h>
#endif

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>


/* defination of BOOL, C is a fw language! */
typedef int bool;
#define true (1)
#define false (0)


/* some useful size */
#define BUF_SIZE    1024
#define PART_SIZE   128
#define MAX_ARGS    64
#define SPLIT_DELIM "/"
#define _SPACE       " "
#define MAX_CMDS    16

#define RUN  (0)
#define DONE (1)

/* Beacuse of it is C, we do not have vector and TMP...*/
typedef struct 
{
    char* data[MAX_ARGS];
    int size;
    int back;
    int infd;
    int outfd;
}str_vector;

bool is_space(char c);

void str_vec_init();

int pushBack(char* str, str_vector* vec);

/* vector ends here */

typedef struct node_t
{
    int job_id;
    char job_name[1024];
    int pid;
    int status;
    struct node_t* next;
}node_t;

void list_init(node_t* node);

void list_insert(node_t* head, int job_id, int pid, char* jobname);

void list_remove(node_t* head, int job_id);

void clear(node_t* head);

void print_list(node_t* head, int length);

void delete_no_run(node_t* head, int* length);
/* link-list ends here */

int getPrompt(char* buf);

void splitBySpace(char* origin, str_vector* container);

int redirect(str_vector* vec, char* infile, char* outfile);

int parse(char* buf, str_vector* cmds, int* back, char* big_buf[64], 
          char* infile, char* outfile, int* app);

#endif 
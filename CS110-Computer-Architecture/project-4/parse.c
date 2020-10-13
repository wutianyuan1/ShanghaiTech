#include "parse.h"

bool is_space(char c){
    return c == ' ';
}

void str_vec_init(str_vector* vec){
    vec->size = 0;
    vec->back = 0;
    vec->infd = 0;
    vec->outfd = 1;
    for (int i = 0; i < MAX_ARGS; ++i) vec->data[i] = NULL;
}

int pushBack(char* str, str_vector* vec){
    if (vec->size == MAX_ARGS) return -1;
    if (str == NULL || *str == '\0') return -1;
    else{
        (vec->data)[vec->back] = str;
        vec->back += 1;
        vec->size += 1;
        return 0;
    }
}

void list_init(node_t* node){
    node->job_id = -1;
    node->job_name[0] = '\0';
    node->pid = -1;
    node->next = NULL;
}

void list_insert(node_t* head, int job_id, int pid, char* jobname){
    node_t* newnode = (node_t*)malloc(sizeof(node_t));
    node_t* iter = head;
    newnode->job_id = job_id;
    newnode->status = 0;
    strcpy( newnode->job_name, jobname);
    newnode->pid = pid;
    while (iter->next != NULL){
        iter = iter->next;
    }
    iter->next = newnode;
    newnode->next = NULL;
}

void list_remove(node_t* head, int pid){
    node_t* iter = head->next;
    node_t* prev = head;
    while (iter != NULL){
        if (iter->pid == pid){
            prev->next = iter->next;
            iter->next = NULL;
            free(iter);
            return;
        }
        iter = iter->next;
        prev = prev->next;
    }
}

void clear(node_t* head){
    if (head->next == NULL) return;
    node_t* iter = head->next;
    node_t* inxt = iter->next;
    while (inxt != NULL){
        free(iter);
        iter = inxt;
        inxt = inxt->next;
    }
    free(iter);
}

//[1]-  Done                    sleep 5
//[2]+  Running                 sleep 4 &

void print_list(node_t* head, int length){
    node_t* iter = head->next;
    char num = ' ';
    int i = 0;
    while ( iter != NULL ){
        char* st;
        num = ' ';
        // printf("printing\n");
        if (i == length - 2) num = '-';
        else if (i == length - 1) num = '+';
        i++;
        if (iter->status == 0){
            st = "Running";
            printf("[%d]%c  %s                 %s\n", iter->job_id, num, st, iter->job_name);
        }
        else if (iter->status == 1) {
            st = "Done";
            iter->job_name[strlen(iter->job_name) - 2] = '\0';
            printf("[%d]%c  %s                    %s\n", iter->job_id, num, st, iter->job_name);
        }

        
        iter = iter->next;
    }
}

void delete_no_run(node_t* head, int* length){
    node_t* iter = head->next; 
    node_t* prev = head;
    while (iter != NULL){
        // printf("deleting\n");
        //printf("deleteing: iter status %d\n", iter->status);
        if (iter->status != RUN){
            node_t* tmp = iter;
            iter = iter->next;
            prev->next = iter;
            free(tmp);
            *length -= 1;
        }else{
            iter = iter->next;
            prev = prev->next;
        }
    }
}


int redirect(str_vector* vec, char* infile, char* outfile){
    int app = 0;
    // printf("argcnt: %d\n", vec->size);
    for (int i = 0; i < vec->size - 1; i++){
        // printf("i = %d\n", i);
        if ( vec->data[i] != NULL && strcmp(vec->data[i], ">") == 0 ){
            strcpy(outfile, vec->data[i+1]);
            vec->data[i] = NULL;
            vec->data[i+1] = NULL;            
            app = 0;
        }
        else if ( vec->data[i] != NULL && strcmp(vec->data[i], ">>") == 0 ){
            strcpy(outfile, vec->data[i+1]);
            vec->data[i] = NULL;
            vec->data[i+1] = NULL;
            app = 1;
        }
        else if ( vec->data[i] != NULL && strcmp(vec->data[i], "<") == 0 ){
            strcpy(infile, vec->data[i+1]);
            vec->data[i] = NULL;
            vec->data[i+1] = NULL;
        }
    }
    return app;
}

int getPrompt(char* buf){
    /* You do not need to know what it is... only to print prompt */
    /* @author: wuty! */
    char hostname[PART_SIZE];
    char copydir[PART_SIZE];
    char curdir[PART_SIZE];
    char* usrname, *first, *second, *real;
    if ( gethostname(hostname, PART_SIZE) == -1 )
        return -1;
    if ( !getcwd(curdir, PART_SIZE) )
        return -1;
    usrname = getpwuid(getuid())->pw_name;
    if ( usrname == NULL )
        return -1;
    for (int i = 0; i < BUF_SIZE; ++i)  buf[i] = '\0';
    strcpy(copydir, curdir);
    first = strtok(copydir, SPLIT_DELIM);
    second = strtok(NULL, SPLIT_DELIM);
    if ( !strcmp(first,"home") && !strcmp(second, usrname) ){
        real = curdir + strlen(first) + strlen(second) + 2;
        sprintf(buf, "%s@%s:~%s$ ", usrname, hostname, real);
    }
    else{
        real = curdir;
        sprintf(buf, "%s@%s:%s$ ", usrname, hostname, curdir);
    }
    return 0;
}

void splitBySpace(char* origin, str_vector* container){
    if (origin == NULL || origin[0] == '\0')
        return;
    char* ptr = origin;
    while (is_space(*ptr)) ptr++;

    pushBack(ptr, container);
    while ( *ptr != '\0' ){
        if ( is_space(*ptr) ){
            *ptr++ = '\0';
            while ( is_space(*ptr) && *ptr != '\0' ) ptr++;
            pushBack(ptr, container);
        }
        ptr++;
    }
}

int parse(char* buf, str_vector* cmds, int* back, char* big_buf[64],
          char* infile, char* outfile, int* append){
    char* iter = buf;
    int cmdcnt = 1;
    char* backptr = buf;
    if (buf == NULL){
        return -1;
    }
    // init all vectors
    for (int i = 0; i < MAX_CMDS; ++i){
        str_vec_init(&cmds[i]);
    }
    // check if it is a background command
    if (buf[strlen(buf) - 1] == '&'){
        buf[strlen(buf) - 1] = '\0';
        *back = 1;
    }
    // split commands by pipe
    while (iter != NULL && *iter != '\0'){
        if (*iter == '|'){
            *iter++ = '\0';
            strcpy(big_buf[cmdcnt - 1], backptr);
            splitBySpace(big_buf[cmdcnt - 1], &(cmds[cmdcnt - 1]));
            backptr = iter;
            cmdcnt++;
        }
        else{
            iter++;
        }
    }
    strcpy(big_buf[cmdcnt - 1], backptr);
    splitBySpace(backptr, &(cmds[cmdcnt - 1]));

    for (int i = 0; i < cmdcnt; i++){
        *append = redirect(&cmds[i], infile, outfile);
    }
    return cmdcnt;
};
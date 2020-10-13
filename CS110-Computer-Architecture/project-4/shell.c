#include <stdio.h>
#include "parse.h"

node_t _head;
node_t* listhead = &_head;
node_t* listtail = &_head;

/* current list length */
int list_len = 0;

char infile[50];
char outfile[50];

str_vector cmds[MAX_CMDS];

void childhld(int sig){
	pid_t pid;
	int status;
	while((pid = waitpid(-1, &status, WNOHANG)) > 0){
		//printf("catch child process, pid:%d\n", pid);
        node_t* iter = listhead;
        while (iter != NULL){
            if (iter->pid == pid) iter->status = DONE;
            iter = iter->next;
        }
    }
	return;
}



int exec_cd_cmd(str_vector* args){
    if (args->size != 2){
        printf("Incorrect args number!\n");
        return -1;
    }
    char* path = args->data[1];
    if ( chdir(path) == -1 ){
        printf("No such file or directory\n");
        return -1;
    }
    return 0;
}

void exec_exit_cmd(){
    clear(listhead);
    exit(EXIT_SUCCESS);
}

void exec_jobs_cmd(){
    print_list(listhead, list_len);
    delete_no_run(listhead, &list_len);
}

void exec_kill_cmd(str_vector* args){
    if (args->size != 2) return;
    signal(SIGQUIT,SIG_DFL);
    int kill_pid = atoi(args->data[1]);
    kill(kill_pid, SIGTERM);
    signal(SIGQUIT,SIG_IGN);
    //list_remove(listhead, kill_pid);
}

bool isInnerCmd(str_vector* args){
    char* cmd = args->data[0];
    if ( strcmp(cmd, "cd") == 0 
      || strcmp(cmd, "jobs") == 0
      || strcmp(cmd, "kill") == 0
      || strcmp(cmd, "exit") == 0 )
      return true;
    return false;
}

void execInnerCmd(str_vector* args){
    char* cmd = args->data[0];
    if ( strcmp(cmd, "cd") == 0 ){
        exec_cd_cmd(args);
    }
    else if ( strcmp(cmd, "jobs") == 0 ){
        exec_jobs_cmd();
    }
    else if ( strcmp(cmd, "kill") == 0 ){
        exec_kill_cmd(args);
    }
    else if ( strcmp(cmd, "exit") == 0 ){
        exec_exit_cmd();
    }
}


int execute(str_vector* argv, int back, int i, int pipes[8][2], int total, char* jobname)
{
    
    fflush(stdout);
    pid_t pid = fork();
    if ( pid != 0 ){
        signal(SIGCHLD, childhld);
        if (back == 1){
            list_insert(listhead, ++list_len, pid, jobname);
            //printf("receive background job\n");
            return 0;
        }else{
            //printf("front job\n");
            waitpid(pid, NULL, 0);
            close(pipes[i][1]);
            return pid;
        }
    }
    else if ( pid == 0 ){
        fflush(stdout);
        if (i == 0){
            // 0 1
            for ( int m = 0; m < total-1; m++){
                for (int n = 0;n < 2; n++){
                    if ( !(m == 0 && n == 1) ) close(pipes[m][n]);
                }
            }
            close(pipes[0][0]);
        }else {
            // i-1, 0 && i, 1
            for ( int m = 0; m < total-1; m++){
                for (int n = 0;n < 2; n++){
                    if ( !((m == i -1 && n == 0) ||  (m == i && n == 1)) ){
                        close (pipes[m][n]);
                    }     
                }
            }
            if ( i == total - 1 ) close(pipes[i][1]);
        }
        if ( back == 1 && argv->infd == 0 )
            argv->infd = open("/dev/null", O_RDONLY);
        if ( argv->infd != 0 ){
            dup2(argv->infd, STDIN_FILENO);
        }
        if ( argv->outfd != 1 ){
            dup2(argv->outfd, STDOUT_FILENO);
        }
        execvp(argv->data[0], argv->data);
    }
    return 0;
}

void execAllCmd(str_vector cmds[], bool append, int total, 
                   char* infile, char* outfile, int back, char* jobname)
{
    str_vector* args = &cmds[total - 1];
    str_vector* first = &cmds[0];
    int fds[total-1][2];
    for (int i = 0; i < total-1; ++i)
        pipe(fds[i]);
    if ( args->size == 0 ) return;
    if ( infile != NULL && infile[0] != '\0' )
        first->infd = open(infile, O_RDONLY);
    if ( outfile != NULL && outfile[0] != '\0' ){
        if ( append )
            args->outfd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
        else
            args->outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }
    if ( back == 1 )
        signal(SIGCHLD, SIG_IGN);
    else
        signal(SIGCHLD, SIG_DFL);
    for ( int i = 0; i < total; i++ ){
        if ( i < total-1 ){
            cmds[i].outfd = fds[i][1];
            cmds[i+1].infd = fds[i][0];
        }
        if ( isInnerCmd(&cmds[i]) ){
            execInnerCmd(&cmds[i]);
        }
        else{
            execute( &cmds[i], back, i, fds, total, jobname );
        }
    }
}

int main(int argc, char** argv){
    list_init(listhead);
    char buf[BUF_SIZE];
    char reads[BUF_SIZE];
    char jobname[BUF_SIZE];
    signal(SIGCHLD, childhld);
    if ( argc != 2 )return 0;
    FILE* fp = fopen(argv[1] , "r");
    while (1){
        fflush(stdout);      
        memset(reads, 0, BUF_SIZE);
        if ( fgets(reads, BUF_SIZE, fp) == NULL ) break;        
        if (reads[0] == 0) continue;
        if (reads[strlen(reads) - 1] == '\n')
            reads[strlen(reads) - 1] = '\0';
        memset(jobname, 0, BUF_SIZE);
        strcpy(jobname, reads);
        // getPrompt(buf);
        // printf("%s", buf);        
        
        int backgnd = 0;
        int app = 0;
        char* tmp[MAX_CMDS];        
        for (int i = 0; i < MAX_CMDS; i++){
            tmp[i] = (char*)malloc(sizeof(char) * BUF_SIZE);
            memset(tmp[i], 0, BUF_SIZE);
        }
        memset(infile, 0, 50);
        memset(outfile, 0, 50);

        int cmd_argc = parse(reads, cmds, &backgnd, tmp, infile, outfile, &app);
        execAllCmd(cmds, app, cmd_argc, infile, outfile, backgnd, jobname);

        for (int i = 0; i < MAX_CMDS; i++){
            free(tmp[i]);
        }
    }
    return 0;
}
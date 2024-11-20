#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "job_list.h"
#include "string_vector.h"
#include "swish_funcs.h"

#define MAX_ARGS 10

int tokenize(char *s, strvec_t *tokens) {
    char *delimiter = " ";

    /*Copy arg to buffer and parse through buffer with delimiter.*/
                                    
    char *token = strtok(s, delimiter);
    
    /*Continue to parse through until end of buffer*/
    while(token != NULL) {
        if (strvec_add(tokens, token) == -1) { 
            perror("strvec_add");
        return -1;
        }  
        token = strtok(NULL, delimiter);  

    }
    return 0;
}

int run_command(strvec_t *tokens) {
    /*First setpgid*/
     if (setpgid(getpid(), getpid()) == -1) { 
        perror("setpgid");
        return -1;
    } 

    struct sigaction sac;
    sac.sa_handler = SIG_DFL;
    if (sigfillset(&sac.sa_mask) == -1) {
        perror("sigfillset");
        return -1;
    }
    sac.sa_flags = 0;
    if (sigaction(SIGTTIN, &sac, NULL) == -1 || sigaction(SIGTTOU, &sac, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    /*Conditionals to check if the user inputted a redirection command*/
    if (strvec_find(tokens, "<") != -1) {
        char* file_title = strvec_get(tokens, strvec_find(tokens, "<") + 1);

        /*Open file for writing, create if needed, otherwise delete existing file
        Grant user read and write permissions for new file*/
        int in_fd = open(file_title, O_RDONLY);
        
        /*Error check to see if file can open*/
        if (in_fd == -1) { 
            perror("Failed to open input file");
            return -1; 
        }

        /*Error check to see if out_fd duplicated*/
        if (dup2(in_fd, 0) == -1) {
            perror("dup2");
            return -1;
        }
    }
    
    if (strvec_find(tokens,">") != -1) {
        /*Open file for writing, create if needed, otherwise delete existing file
        Grant user read and write permissions for new file*/
        char* file_title = strvec_get(tokens, strvec_find(tokens, ">") + 1);
        int out_fd = open(file_title, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
        
        /*Error check to see if file can open*/
        if (out_fd == -1) { 
            perror("Failed to open output file");
            return -1; 
        }

        /*Error check to see if out_fd duplicated*/
        if (dup2(out_fd, 1) == -1) {
            perror("dup2");
            return -1;
        }
    }
    if (strvec_find(tokens,">>") != -1) {
        /*Open file for writing, create if needed, otherwise delete existing file
        Grant user read and write permissions for new file*/
        char* file_title = strvec_get(tokens, strvec_find(tokens, ">>") + 1);

        int out_fd = open(file_title, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
        
        /*Error check to see if file can open*/
        if (out_fd == -1) { 
            perror("Failed to open output file");
            return -1; 
        }

        /*Error check to see if out_fd duplicated*/
        if (dup2(out_fd, 1) == -1) {
            perror("dup2");
            return -1;
        }
    } 

    char *tokens_array[MAX_ARGS];
    int j = 0; 
    while (j < tokens->length)  {
        /*Create a conditional finding the if the first instance of the redirection. 
        If found, break out of the loop. Else we add the value into a character array 
        to exec*/
        if (strcmp(strvec_get(tokens, j), "<") == 0 || strcmp(strvec_get(tokens, j), ">") == 0 || strcmp(strvec_get(tokens, j), ">>") == 0) {
            break;
        }
            tokens_array[j] = strvec_get(tokens, j);
            j++;
        }
    

    /*Error checking to see if exec worked*/
    tokens_array[j] = NULL;
    if (execvp(tokens_array[0], tokens_array) == -1) {
        perror ("exec");
        return -1;
    } 
    // Not reachable after a successful exec(), but retain here to keep compiler happy
    return 0;
}

int resume_job(strvec_t *tokens, job_list_t *jobs, int is_foreground) {    
    /*Gets the index supplied by user and error check if it works*/
    int listnum = atoi(strvec_get(tokens, 1));
    int status;
    /*checks if the index token is less than or greater than the list indicies*/
    if(listnum >= jobs->length || listnum < 0){
        printf("Job index out of bounds\n");
        return -1;
    }

    /*This allows the job to resume*/
    job_t *resjob = job_list_get(jobs, listnum);
    if (resjob == NULL) { 
        perror("Error: Unable to job_list_get()");
        return -1;
    }

    /*0 is foreground, 1 is background*/
    if(is_foreground && tcsetpgrp(STDIN_FILENO, resjob->pid)){
        perror("file is not in foreground.\n");
        return -1;
    }
    /*allows job to resume*/
    kill(resjob->pid, SIGCONT);
    /*waits if foreground process, giving same error checks as main*/
    if(is_foreground){
        if(waitpid(resjob->pid, &status, WUNTRACED) == -1){
            perror("Unable to waitpid().\n");
            return -1;
        }
        /*if job terminated*/
        if(WIFEXITED(status) || WIFSIGNALED(status)){
            job_list_remove(jobs, listnum);
        }

        else if(WIFSTOPPED(status)){
            resjob->status = JOB_STOPPED;
        }
        tcsetpgrp(STDIN_FILENO, getpid());
    }
    else{
        /*simply resumes job in background if is_forground equals 1*/
        resjob->status = JOB_BACKGROUND;
        kill(resjob->pid, SIGCONT);
    }
    return 0;
}

int await_background_job(strvec_t *tokens, job_list_t *jobs) {
    /*creates variables for checks and functions later*/
    int listnum = atoi(strvec_get(tokens, 1));
    int status;
    /*checks if index is in bounds*/
    if(listnum >= jobs->length || listnum < 0){
        printf("Job index out of bounds\n");
        return -1;
    }
    /*creates pointer to job in list*/
    job_t *resjob = job_list_get(jobs, listnum);
    /*Error check to see if job is in background*/
    if(resjob->status != JOB_BACKGROUND){
        printf("Job index is for stopped process not background process\n");
        return -1;
    }

    if(waitpid(resjob->pid, &status, WUNTRACED) == -1){
        perror("Unable to waitpid().\n");
        return -1;
    }
    /*Error check to see if wait failed and removes job from list once terminated*/
    if(WIFEXITED(status) || WIFSIGNALED(status)){
        job_list_remove(jobs, listnum);
    }

    return 0;
}

int await_all_background_jobs(job_list_t *jobs) {
    /*only need status variable as we need to iterate through whole list*/
    int status;
    if(jobs->length <= 0){
        perror("No jobs in background.\n");
    }
    /*gets the job at the first index to iterate through*/
    job_t *curjob = job_list_get(jobs, 0);

    for(int i = 0; i < jobs->length; i++){
        /*skips job if stopped*/
        if(curjob->status == JOB_STOPPED){
            curjob = curjob->next;
        }
        else{
            if(waitpid(curjob->pid, &status, WUNTRACED) == -1){
                perror("Unable to waitpid().\n");
                return -1;
            }
            /*Error checks and changes status if job was stopped once again*/
            if(WIFSTOPPED(status)){
                curjob->status = JOB_STOPPED;
            }
            curjob = curjob->next;
        }
    }
    /*removes any job in background as any job that is not terminated has the JOB_STOPPED status*/
    job_list_remove_by_status(jobs, JOB_BACKGROUND);
    return 0;
}
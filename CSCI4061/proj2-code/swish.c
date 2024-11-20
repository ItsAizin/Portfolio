#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "job_list.h"
#include "string_vector.h"
#include "swish_funcs.h"

#define CMD_LEN 512
#define PROMPT "@> "

int main(int argc, char **argv) {
    struct sigaction sac;
    sac.sa_handler = SIG_IGN;
    if (sigfillset(&sac.sa_mask) == -1) {
        perror("sigfillset");
        return 1;
        
    }
    sac.sa_flags = 0;
    if (sigaction(SIGTTIN, &sac, NULL) == -1 || sigaction(SIGTTOU, &sac, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    strvec_t tokens;
    strvec_init(&tokens);
    job_list_t jobs;
    job_list_init(&jobs);
    char cmd[CMD_LEN];

    printf("%s", PROMPT);
    while (fgets(cmd, CMD_LEN, stdin) != NULL) {
        // Need to remove trailing '\n' from cmd. There are fancier ways.
        int i = 0;
        while (cmd[i] != '\n') {
            i++;
        }
        cmd[i] = '\0';

        if (tokenize(cmd, &tokens) != 0) {
            printf("Failed to parse command\n");
            strvec_clear(&tokens);
            job_list_free(&jobs);
            return 1;        
        }
        if (tokens.length == 0) {
            printf("%s", PROMPT);
            continue;
        }
        const char *first_token = strvec_get(&tokens, 0);

        if (strcmp(first_token, "pwd") == 0) {
            /*Create a buffer for the current directory name*/
            char cur_Directory[CMD_LEN];

            /*Error checking, if current directory is found, then we print it. Else we print error*/
            if (getcwd(cur_Directory, sizeof(cur_Directory)) == NULL) { 
                perror("getcwd");
                continue;
            } else { 
                printf("%s\n", cur_Directory);

            }

       } else if (strcmp(first_token, "cd") == 0) {
            /*First determine if user provided a directory. If not,
            set it to the home directory*/
            if (strvec_get(&tokens, 1) != NULL) { 
                /*Error check to see if changing directory using the user's
                input works*/
                if (chdir(strvec_get(&tokens, 1)) == -1) {
                    perror("chdir");
                    job_list_free(&jobs);
                    strvec_clear(&tokens);
                }
            } else {
                if (chdir(getenv("HOME")) == -1) {
                    perror("chdir");
                    job_list_free(&jobs);
                    strvec_clear(&tokens);
                }
            }
       }
        else if (strcmp(first_token, "exit") == 0) {
            strvec_clear(&tokens);
            break;
        }

        // Task 5: Print out current list of pending jobs
        else if (strcmp(first_token, "jobs") == 0) {
            int i = 0;
            job_t *current = jobs.head;
            while (current != NULL) {
                char *status_desc;
                if (current->status == JOB_BACKGROUND) {
                    status_desc = "background";
                } else {
                    status_desc = "stopped";
                }
                printf("%d: %s (%s)\n", i, current->name, status_desc);
                i++;
                current = current->next;
            }
        }

        // Task 5: Move stopped job into foreground
        else if (strcmp(first_token, "fg") == 0) {
            if (resume_job(&tokens, &jobs, 1) == -1) {
                printf("Failed to resume job in foreground\n");
            }
        }

        // Task 6: Move stopped job into background
        else if (strcmp(first_token, "bg") == 0) {
            if (resume_job(&tokens, &jobs, 0) == -1) {
                printf("Failed to resume job in background\n");
            }
        }

        // Task 6: Wait for a specific job identified by its index in job list
        else if (strcmp(first_token, "wait-for") == 0) {
            if (await_background_job(&tokens, &jobs) == -1) {
                printf("Failed to wait for background job\n");
            }
        }

        // Task 6: Wait for all background jobs
        else if (strcmp(first_token, "wait-all") == 0) {
            if (await_all_background_jobs(&jobs) == -1) {
                printf("Failed to wait for all background jobs\n");
            }
        }

        else {
            /*Create variables to for the child process and wait status*/
            int status;
            pid_t pid = fork();

            /*Error checking and determining if it's the child or parent process*/
            if (pid == -1) {
                perror("Error: Unable to fork().");
                job_list_free(&jobs);
                strvec_clear(&tokens);
                return 1;
                
            } else if (pid == 0) { 
                /*Wait the child process and error check. If result of child variable isn't -1, 
                then call run_command and error check the results*/
                int command_result = run_command(&tokens);

                /*Error check to see if the run_command experienced errors*/
                if (command_result == -1) {
                    job_list_free(&jobs);
                    strvec_clear(&tokens); 
                    return 1;
                }
                
            } else { 
                /*checks if & is not the last token which sets a foreground job*/
                if(strcmp(strvec_get(&tokens, tokens.length - 1), "&") == 1){
                    if (tcsetpgrp(STDIN_FILENO, pid) == -1) { 
                        perror("Error: Unable to tcsetpgrp().\n");
                        job_list_free(&jobs);
                        strvec_clear(&tokens);
                        return 1;
                    }
                    
                    /*Error check waitpid*/
                    if (waitpid(pid, &status, WUNTRACED) == -1) { 
                        perror("Error: Unable to waitpid().\n");
                        job_list_free(&jobs);
                        strvec_clear(&tokens);
                        return 1;
                    }
                    /*Edds job to stopped list if recieved signal to stop as job would either be background job or terminated*/
                    if(WIFSTOPPED(status)){
                        /*Error checks if job list add fails*/
                        if(job_list_add(&jobs, pid, strvec_get(&tokens, 0), JOB_STOPPED) == -1) {
                            perror("Error: job_list_add");
                            job_list_free(&jobs);
                            strvec_clear(&tokens);
                            return 1;
                        }
                    }
                    
                    if (tcsetpgrp(STDIN_FILENO, getpid()) == -1) { 
                        perror("Error: Unable to tcsetpgrp().\n");
                        job_list_free(&jobs);
                        strvec_clear(&tokens);
                        return 1;
                    }
                } else { 
                    /*adds job to background list and does not wait for child*/
                    /*Error checks if job list add fails*/
                    strvec_take(&tokens, tokens.length -1);
                    if(job_list_add(&jobs, pid, strvec_get(&tokens, 0), JOB_BACKGROUND) == -1) {
                        perror("Error: job_list_add");
                        job_list_free(&jobs);
                        strvec_clear(&tokens);
                        return 1;
                    }
                }
            }
        }
    strvec_clear(&tokens);  
    printf("%s", PROMPT);
    }
    /*Erases job list in end to prevent memory leaks*/
    job_list_free(&jobs);
    return 0;
}
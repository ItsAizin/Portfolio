#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "string_vector.h"
#include "swish_funcs.h"

#define MAX_ARGS 10

/*
 * Helper function to run a single command within a pipeline. You should make
 * make use of the provided 'run_command' function here.
 * tokens: String vector containing the tokens representing the command to be
 * executed, possible redirection, and the command's arguments.
 * pipes: An array of pipe file descriptors.
 * n_pipes: Length of the 'pipes' array
 * in_idx: Index of the file descriptor in the array from which the program
 *         should read its input, or -1 if input should not be read from a pipe.
 * out_idx: Index of the file descriptor in the array to which the program
 *          should write its output, or -1 if output should not be written to
 *          a pipe.
 * Returns 0 on success or -1 on error.
 */


int run_piped_command(strvec_t *tokens, int *pipes, int n_pipes, int in_idx, int out_idx) {  
    pid_t cid = fork();

    if (cid == -1) {
        perror("fork");

        return -1;
    } else if (cid == 0) {
        /*Close the unused pipes*/
        for (int i = 0; i < n_pipes; i++) {
            if (i != in_idx && i != out_idx) {
                close(pipes[i]);
            }
        }

        /*Determine if we should read from pipe*/
        if (in_idx != -1) {
            /*duplicate the process*/
            int dup_in = dup2(pipes[in_idx], STDIN_FILENO);
            
            if (dup_in == -1) {
                perror("dup2");
                exit(1);            
            } 
      
        }

        /*Determine if we should write from pipe*/
        if (out_idx != -1) {
            /*duplicate the process*/
            int dup_out = dup2(pipes[out_idx], STDOUT_FILENO);
            
            if (dup_out == -1) {
                perror("dup2");
                exit(1);            
            }        
        }

        /*run command on redirections*/
        int run_result = run_command(tokens);

        if(run_result == -1) {
            perror("run_command");
            exit(1);            
        }

        exit(0);            
    } else {
        return 0;
    }
}

/*Initialize an array of pipes and check if 
array of pipes was correctly created*/
int run_pipelined_commands(strvec_t *tokens) {
    strvec_t sliced;
    int num_children = strvec_num_occurrences(tokens, "|") + 1;

    int pipe_fds[2 * num_children];
    for (int i = 0; i < num_children; i++) {
        if (pipe(pipe_fds + 2 * i) == -1) {
            perror("pipe");
            for (int j = 0; j < 2 * num_children; j++) {
                close(pipe_fds[j]);
            }
            exit(1);
        }
    }

    int find_result = strvec_find_last(tokens, "|");

    if (find_result == -1) {
        perror("strvec_find");
        for (int j = 0; j < 2 * num_children; j++) {
            close(pipe_fds[j]);
        }
        exit(1);
    }

    /*Slice the tokens array for the right-most command. Starts at the results and ends 
    at the end fo the tokens + 1 (exclusive). Error check if the result works*/
    int slice_result = strvec_slice(tokens, &sliced, find_result + 1, tokens->length);

    if (slice_result == -1) {
        perror("strvec_slice");
        for (int j = 0; j < 2 * num_children; j++) {
            close(pipe_fds[j]);
        }
        exit(1);
    }

    /*Calculates right-most end of commands and clears afterwards*/
    int piped_command_result = run_piped_command(&sliced, pipe_fds, 2 * num_children, 2 * (num_children - 2), -1);

    if (piped_command_result == -1) {
        perror("run_piped_command");
        for (int j = 0; j < 2 * num_children; j++) {
            close(pipe_fds[j]);
        }    
        strvec_clear(&sliced);
        exit(1); 
    }

    /*Clear the temp string vector and remove the commands we've used*/
    strvec_take(tokens, find_result);
    strvec_clear(&sliced);

    for (int i = num_children - 1; i > 1; i--) {
        /*Create a string vector to hold contents of command at the index of the last occurence of | to the end
        of the tokens string vector and error check*/
        int slice_result = strvec_slice(tokens, &sliced, strvec_find_last(tokens, "|") + 1, tokens->length);
        
        if (slice_result == -1) {
            perror("strvec_slice");
            for (int j = 0; j < 2 * num_children; j++) {
                close(pipe_fds[j]);
            }    
            strvec_clear(&sliced);
            exit(1);    
        }

        /*Run sliced commands and error check*/
        piped_command_result = run_piped_command(&sliced, pipe_fds, 2 * num_children, 2 * (i - 2), 2 * i - 1);

        if (piped_command_result == -1) {
            perror("run_piped_command");
            for (int j = 0; j < 2 * num_children; j++) {
                close(pipe_fds[j]);
            }    
            strvec_clear(&sliced);
            exit(1); 
        }

        /*Clear the temp string vector and remove the commands we've used*/
        strvec_clear(&sliced);
        strvec_take(tokens, strvec_find_last(tokens, "|"));
    }

    if (find_result == -1) {
        perror("strvec_find");
        for (int j = 0; j < 2 * num_children; j++) {
            close(pipe_fds[j]);
        }
        exit(1);
    }


    /*Slice the tokens array for the left-most command. Starts at the 0 and ends 
    at the end fo the tokens.length (exclusive). Error check if the result works*/
    slice_result = strvec_slice(tokens, &sliced, 0, tokens->length);

    if (slice_result == -1) {
        perror("strvec_slice");
        for (int j = 0; j < 2 * num_children; j++) {
            close(pipe_fds[j]);
        }
        exit(1);
    }

    /*Calculates left-most end of commands and error check*/
    piped_command_result = run_piped_command(&sliced, pipe_fds, 2 * num_children, -1, 1);

    if (piped_command_result == -1) {
        perror("run_piped_command");
        for (int j = 0; j < 2 * num_children; j++) {
            close(pipe_fds[j]);
        }    
        strvec_clear(&sliced);
        exit(1); 
    }

    strvec_clear(&sliced);

    for (int j = 0; j < 2 * num_children; j++) {
        close(pipe_fds[j]);
    }

    /*close pipe ends and wait for children*/
    int all_return = 0;
    for(int i = 0; i < num_children; i++){
        if(wait(NULL) == 1){
	        all_return = 1;
        }
    }
    if (all_return == 1){
        perror("child has failed");
        return 1;
    }
    return 0;
}
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_WORD_LEN 25

/*
 * Counts the number of occurrences of words of different lengths in a text
 * file and stores the results in an array.
 * file_name: The name of the text file from which to read words
 * counts: An array of integers storing the number of words of each possible
 *     length.  counts[0] is the number of 1-character words, counts [1] is the
 *     number of 2-character words, and so on.
 * Returns 0 on success or -1 on error.
 */
int count_word_lengths(const char *file_name, int *counts) {
        FILE *file_in = fopen(file_name, "r");
        
        /*Error check to see if file can open*/
        if (file_in == NULL) {
            perror("fopen");
            return -1; 
        }

        /*Read until EOF, and check if the word is a whitespace or not.
        If not, increment i indicating number of letters before whitespace.
        Else, increment the value of counts at index i */
        char char_check = fgetc(file_in);
        int i = 0;
        while(char_check != EOF) {
            if (islower(char_check) != 0) {
                i++;
            } else if (islower(char_check) == 0) {
                counts[i - 1]++;
                i = 0;
            }
            char_check = fgetc(file_in);
        }

        /*Error check to see if the EOF was really an end of file. 
        Or an error that was encountered*/
        if (ferror(file_in)) {
            perror("ferror");
            fclose(file_in);
            return -1;
        }

    fclose(file_in);
    return 0;
}

/*
 * Processes a particular file (counting the number of words of each length)
 * and writes the results to a file descriptor.
 * This function should be called in child processes.
 * file_name: The name of the file to analyze.
 * out_fd: The file descriptor to which results are written
 * Returns 0 on success or -1 on error
 */
int process_file(const char *file_name, int out_fd) {
    /*allocates enough space to count words of every length and returns if
    not enough space is available*/
    int *counter = malloc(sizeof(int)*MAX_WORD_LEN);
    memset(counter, 0, sizeof(int)*MAX_WORD_LEN);
    if(counter == NULL){
        perror("malloc");
        free(counter);
        return -1;
    }

    /*runs count word lengths and writes results to counter or
    returns on failure*/
    if(count_word_lengths(file_name, counter) == -1){
        free(counter);
        return -1;
    }
    /*writes the result of count_word_length to out_fd*/
    if(write(out_fd, counter, sizeof(int)*MAX_WORD_LEN) < 0){
        perror("write");
        free(counter);
        return -1;
    }
    free(counter);
    return 0;
}

/*TODO:
    - FINISH ANALYZING EACH FILE IN CHILD
    - FINISH CHANGING CODE IN PARENT TO COMBINE AND DISPLAY AGGREGATE RESULTS
    - FINISH ERROR CHECKING*/
int main(int argc, char **argv) {
    if (argc == 1) {
        // No files to consume, return immediately
        return 0;
    }

    /*Create a pipe to use for every child and returns if failed to create*/
    /*creates pipe_sums and counter to track length of words in every file later*/
    int num_children = argc - 1;
    int pipe_sums[MAX_WORD_LEN];
    int counter[MAX_WORD_LEN];
    int pipe_fds[2];
    int ret = pipe(pipe_fds);
    if(ret < 0){
        perror("pipe");
        return -1;
    }
    /*fills both arrays with 0 in case of other functions failing*/
    for (int i = 0; i < MAX_WORD_LEN; i++){
        pipe_sums[i] = 0;
        counter[i] = 0;
    }
    for (int i = 0; i < num_children; i++) {
        /*Create a fork for every file and return if fork failed
	or run process_file if it is the child process*/
        pid_t cid = fork();
        if (cid == -1) {
            perror("fork");
            close(pipe_fds[0]);
            close(pipe_fds[1]);
            return -1;
        }
	/*close the read end of the pipe and check if process file had failed*/
        else if (cid == 0) {
            close(pipe_fds[0]);

            if(process_file(argv[1+i], pipe_fds[1]) == -1){
                close(pipe_fds[1]);
                exit(1);
            }
        close(pipe_fds[1]);
        exit(0);
        }
    }
    /*adds the result from every child into the pipe_sums array using counter*/
    close(pipe_fds[1]);
    for(int i = 0; i < num_children; i++){
	    if(read(pipe_fds[0], counter, sizeof(int)*MAX_WORD_LEN) < 0){
	        perror("read to counter");
	        close(pipe_fds[0]);
	        return -1;
        }

	    for(int j = 0; j < MAX_WORD_LEN; j++){
	        pipe_sums[j] += counter[j];
	    }
    }
    /*parent process waits for every child to finish*/
    int all_return = 0;
    for(int i = 0; i < num_children; i++){
	    if(wait(NULL) == -1){
	        all_return = 1;
        }
    }
    if(all_return == 1){
	    close(pipe_fds[0]);
    }
    close(pipe_fds[0]);
        /*print all pipe_sums results together*/
        for (int i = 1; i <= MAX_WORD_LEN; i++) {
            printf("%d-Character Words: %d\n", i, pipe_sums[i-1]);
        }
    return 0;
    }
    // TODO Fork a child to analyze each specified file (names are argv[1], argv[2], ...)
    // TODO Aggregate all the results together by reading from the pipe in the parent

       

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "http.h"

#define BUFSIZE 512

const char *get_mime_type(const char *file_extension) {
    if (strcmp(".txt", file_extension) == 0) {
        return "text/plain";
    } else if (strcmp(".html", file_extension) == 0) {
        return "text/html";
    } else if (strcmp(".jpg", file_extension) == 0) {
        return "image/jpeg";
    } else if (strcmp(".png", file_extension) == 0) {
        return "image/png";
    } else if (strcmp(".pdf", file_extension) == 0) {
        return "application/pdf";
    }

    return NULL;
}

int read_http_request(int fd, char *resource_name) {
    /*duplicate process to run independently*/
    int fd_copy = dup(fd);
    if (fd_copy == -1) {
        perror("dup");
        return -1;
    }

    /*Open the duplicated process and error check to see if it opened correctly*/
    FILE *socket_stream = fdopen(fd_copy, "r");
    if (socket_stream == NULL) {
        perror("fdopen");
        close(fd_copy);
        return -1;
    }
    // Disable the usual stdio buffering
    if (setvbuf(socket_stream, NULL, _IONBF, 0) != 0) {
        perror("setvbuf");
        // fclose(socket_stream);
        return -1;
    }

    /*Create character arrays as a buffer and the file_name for returning*/
    char buf[BUFSIZE];
    char string_to_parse[BUFSIZE];
    char *delimiter = " ";

    /*Read the first line, to parse only the file name. 
    Then read the remaining lines*/
    char *first_line = fgets(buf, BUFSIZE, socket_stream);

    strcpy(string_to_parse, first_line);
    char *token = strtok(string_to_parse, delimiter);

    for (int i = 0; i < 1; i++) {
        token = strtok(NULL, delimiter);
    }

    strcpy(resource_name, token);

    while (fgets(buf, BUFSIZE, socket_stream) != NULL) {
        if (strcmp(buf, "\r\n") == 0) {
            break;
        }
    }

    /*Simple cleanup and error checking*/
    if (fclose(socket_stream) != 0) {
        perror("fclose");
        return -1;
    }


    return 0;
}

//call lenght and type like a long string
//write until no more data
int write_http_response(int fd, const char *resource_path) {
    /*In the case where the resource path is incorrect*/
    char buf[BUFSIZE];
    struct stat stat_buf;
    if(stat(resource_path, &stat_buf) != 0){
        snprintf(buf, BUFSIZE, "HTTP/1.0 404 Not Found\r\n");
        write(fd, buf, strlen(buf));
        snprintf(buf, BUFSIZE, "Content-Length: 0\r\n");
        write(fd, buf, strlen(buf));
        snprintf(buf, BUFSIZE, "\r\n");
        write(fd, buf, strlen(buf));
        return -1;
    }

    /*use strrchr to get the last occurence of . in the resource path
    then use it to get the tyupe for later use.*/
    const char *cont_type = get_mime_type(strrchr(resource_path, '.'));

    snprintf(buf, BUFSIZE, "HTTP/1.0 200 OK\r\n");
    write(fd, buf, strlen(buf));
    snprintf(buf, BUFSIZE, "Content-Type: %s\r\n", cont_type);
    write(fd, buf, strlen(buf));
    snprintf(buf, BUFSIZE, "Content-Length: %ld\r\n\r\n", stat_buf.st_size);
    write(fd, buf, strlen(buf));

    /*Open the resource path for later and error check*/
    int file_fd = open(resource_path, O_RDONLY);
    if(file_fd == -1){
        perror("fopen");
        close(file_fd);
        return -1;
    }
    
    //fread and write can be used, similar to project 1
    /*You should write out all the contents in chuncks*/
    /*specify more of the contents*/
    int read_val = 0;
    while((read_val = read(file_fd, buf, BUFSIZE)) > 0) {
        if (write(fd, buf, read_val) == -1) { 
            perror("write");
            close(file_fd);
            close(fd);
            return -1;
        }
    }
    close(file_fd);
    return 0;
}
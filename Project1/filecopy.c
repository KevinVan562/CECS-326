/**
 * filecopy.c
 * 
 * This program copies files using a pipe.
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[])
{
    // Check to see if there is enough command-line arguments
    if (argc != 3) {
        printf("Error: must have 3 command-line arguments\n");
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    // Check if source file exists 
    if (access(argv[1], F_OK) == -1) {
        fprintf(stderr, "Error: Unable to open source file '%s'.\n", argv[1]);
        return 1;
    }    

    // Create pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Pipe failed\n");
        return 1;
    }
    
    // Create process
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed\n");
        return 1;
    }
    
    // Parent Process
    if (pid > 0) {
        close(pipefd[READ_END]);    // Close unused read end

        int src_fd = open(argv[1], O_RDONLY);
        if (src_fd == -1) {
            perror("Error opening source file\n");
            close(pipefd[WRITE_END]);
            return 1;
        }

        char c;
        // Read from source file and write to pipe
        while (read(src_fd, &c, 1) > 0) {
            write(pipefd[WRITE_END], &c, 1);
        }

        close(src_fd);
        close(pipefd[WRITE_END]);   // Close write end
        wait(NULL);
    } else {    
        // Child Process
        close(pipefd[WRITE_END]);   // Close unused write end

        // If there is no destination file, create destination file
        int dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (dest_fd == -1) {
            perror("Error opening destination file\n");
            return 1;
        }

        char c;
        // Read from pipe and write to destination file
        while(read(pipefd[READ_END], &c, 1) > 0) {
            write(dest_fd, &c, 1);
        }

        close(dest_fd);
        close(pipefd[READ_END]);   // Close read end

        // Success message if operation was completed
        printf("File successfully copied from %s to %s.\n", argv[1], argv[2]);
    }

    return 0;
}

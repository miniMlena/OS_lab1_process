#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(void) {
    char filename[4096];

    printf("[parent] Введите имя файла: ");
    fflush(stdout);

    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        fprintf(stderr, "[parent] Не удалось прочитать имя файла\n");
        return EXIT_FAILURE;
    }

    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }
    if (filename[0] == '\0') {
        fprintf(stderr, "[parent] Имя файла пустое\n");
        return EXIT_FAILURE;
    }

    int file_fd = open(filename, O_RDONLY); //что такое символ (типа .. symbol)
    if (file_fd == -1) {
        fprintf(stderr, "[parent] Не удалось открыть файл '%s': %s\n", filename, strerror(errno));
        return EXIT_FAILURE;
    }

    int pipe1[2];
    if (pipe(pipe1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {

        if (dup2(file_fd, STDIN_FILENO) == -1) {
            perror("[child] dup2 stdin");
            _exit(EXIT_FAILURE);
        }

        if (dup2(pipe1[1], STDOUT_FILENO) == -1) {
            perror("[child] dup2 stdout");
            _exit(EXIT_FAILURE);
        }

        close(file_fd);
        close(pipe1[0]);
        close(pipe1[1]); // зачем и то, и то закрыли

        execl("./child", "./child", (char *)NULL);
        // почему не делаем проверку как выше, 

        perror("[child] execl");
        _exit(EXIT_FAILURE);
    }
    close(file_fd);
    
    close(pipe1[1]);

    char buf[4096];
    ssize_t n;
    while ((n = read(pipe1[0], buf, sizeof(buf))) > 0) {
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(STDOUT_FILENO, buf + written, (size_t)(n - written));
            if (w == -1) {
                if (errno == EINTR) continue;
                perror("[parent] write");
                close(pipe1[0]);
                return EXIT_FAILURE;
            }
            written += w;
        }
    }

    if (n == -1) {
        perror("[parent] read");
        close(pipe1[0]);
        return EXIT_FAILURE;
    }

    close(pipe1[0]);

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
        
    }

    if (WIFEXITED(status)) {
        printf("[parent] Дочерний процесс завершился с кодом %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[parent] Дочерний процесс убит сигналом %d\n", WTERMSIG(status));
    }

    return EXIT_SUCCESS;
}
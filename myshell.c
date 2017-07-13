#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

struct history {
    char command[256];
    int index;
};

struct history myhis[20];

int num;

void cd(char* command);
void history(int index);
void multi(char* command);
void re(char* command);
void double_re(char* command);
void over_re(char* command);
void left_re(char* command);
void piping(char* command);
void normal(char* command);

int main(void) {

    int stat;
    num = 0;
    int amp = 2;
    int back = 0;

    while(1) {
        int len = 0;
        int pid;
        char input;

        char command[256];
        char pwd[256];
        getcwd(pwd, 256);

        if(amp == 0) {
            wait(NULL);
            back--;
            if(back < 0) {
                back = 0;
            }
        }
        if((amp == 1) && (back > 1)) {
            wait(NULL);
            back--;
        }

        printf("smsh %s : ", pwd);
        while((input = getchar()) != '\n') {
            command[len] = input;
            len++;
        }

        command[len] = '\0';
        strcpy(myhis[num % 20].command, command);
        myhis[num % 20].index = num;
        num++;



        if(strcmp(command, "exit") == 0) {
            break;
        }

        if(strstr(command, "&")) {
            amp = 1;
            back++;
            strtok(command, "&");
        }
        else {
            amp = 0;
        }

        if(strstr(command, ";")) {
            multi(command);
        }
        else if(strstr(command, "cd")) {
            cd(command);
        }
        else if(strstr(command, "history")) {
            history(num);
        }
        else {
            if((pid = fork()) == 0) {
                if(strstr(command, ">")) {
                    if(strstr(command, ">>")) {
                        double_re(command);
                    }
                    else if(strstr(command, ">!")) {
                        over_re(command);
                    }
                    else {
                        re(command);
                    }
                }
                else if(strstr(command, "<")) {
                    left_re(command);
                }
                else if(strstr(command, "|")) {
                    piping(command);
                }
                else {
                    normal(command);
                }
            }

            if(amp == 0) {
                wait(&stat);
            }
        }
    }

    return 0;
}

void cd(char* command) {
    char temp[256];
    char path[256];
    char path2[256];

    strcpy(temp, command);

    if(strcmp(temp, "cd") == 0) {
        chdir(getenv("HOME"));
    }
    else {
        strtok(temp, " ");
        strcpy(path, strtok(NULL, " "));

        if(strcmp(path, "~") == 0) {
            chdir(getenv("HOME"));
        }
        else if(strstr(path, "~")) {
            chdir(getenv("HOME"));
            strtok(path, "/");
            strcpy(path2, strtok(NULL, "/"));
            chdir(path2);
        }
        else {
            chdir(path);
        }
    }
}

void history(int num) {
    int i;

    if(num <= 20) {
        for(i = 0; i < num; i++) {
            printf("%d %s\n", myhis[i].index, myhis[i].command);
        }
    }
    else {
        for(i = (num % 20); i < 20; i++) {
            printf("%d %s\n", myhis[i].index, myhis[i].command);
        }

        for(i = 0; i < (num % 20); i++) {
            printf("%d %s\n", myhis[i].index, myhis[i].command);
        }
    }
}

void multi(char* command) {
    char temp[256];
    char* tok[256];
    int i = 1, j = 0, k;
    int stat;
    strcpy(temp, command);

    tok[0] = strtok(temp, ";");
    while((tok[i] = strtok(NULL, ";")) != NULL) {
        i++;
    }

    while(tok[j] != NULL) {
        if(strstr(tok[j], "cd")) {
            cd(tok[j]);
        }
        if(strstr(tok[j], "history")) {
            history(num);
        }
        else {
            if(fork() == 0) {
                if(strstr(tok[j], ">")) {
                    if(strstr(tok[j], ">>")) {
                        double_re(tok[j]);
                    }
                    else if(strstr(tok[j], ">!")) {
                        over_re(tok[j]);
                    }
                    else {
                        re(tok[j]);
                    }
                }
                else if(strstr(tok[j], "<")) {
                    left_re(tok[j]);
                }
                else if(strstr(tok[j], "|")) {
                    piping(tok[j]);
                }
                else {
                    normal(tok[j]);
                }
            }
            else {
                wait(&stat);
            }
        }

        j++;
    }
}

void re(char* command) {
    char temp[256];
    char* tok[256];
    int fd;

    strcpy(temp, command);

    tok[0] = strtok(temp, ">");
    tok[1] = strtok(NULL, ">");
    tok[1] = strtok(tok[1], " ");

    fd = open(tok[1], O_RDWR | O_TRUNC | O_CREAT , 0777);
    close(1);
    dup(fd);
    close(fd);
    normal(tok[0]);
}

void double_re(char* command) {
    char temp[256];
    char* tok[256];
    int fd;

    strcpy(temp, command);

    tok[0] = strtok(temp, ">>");
    tok[1] = strtok(NULL, ">>");
    tok[1] = strtok(tok[1], " ");

    fd = open(tok[1], O_RDWR | O_CREAT | O_APPEND, 0777);
    close(1);
    dup(fd);
    close(fd);
    normal(tok[0]);
}

void over_re(char* command) {
    char temp[256];
    char* tok[256];
    int fd;

    strcpy(temp, command);

    tok[0] = strtok(temp, ">!");
    tok[1] = strtok(NULL, ">!");
    tok[1] = strtok(tok[1], " ");

    unlink(tok[1]);
    fd = open(tok[1], O_RDWR | O_CREAT , 0777);
    close(1);
    dup(fd);
    close(fd);
    normal(tok[0]);
}

void left_re(char* command) {
    char temp[256];
    char* tok[256];
    int fd;

    strcpy(temp, command);

    tok[0] = strtok(temp, "<");
    tok[1] = strtok(NULL, "<");
    tok[1] = strtok(tok[1], " ");

    fd = open(tok[1], O_RDWR);
    close(0);
    dup(fd);
    close(fd);
    normal(tok[0]);
}

void piping(char* command) {
    char temp[256];
    char* tok[256];

    int i = 1, j = 1;
    int file[2];

    strcpy(temp, command);

    tok[0] = strtok(temp, "|");
    tok[1] = strtok(NULL, "");

    if(strstr(tok[1], "|")) {
        pipe(file);

        if(fork() == 0) {
            close(1);
            dup(file[1]);
            close(file[1]);
            close(file[0]);
            normal(tok[0]);
        }

        close(0);
        dup(file[0]);
        close(file[0]);
        close(file[1]);
        piping(tok[1]);
    }
    else {
        pipe(file);

        if(fork() == 0) {
            close(1);
            dup(file[1]);
            close(file[1]);
            close(file[0]);
            normal(tok[0]);
        }

        close(0);
        dup(file[0]);
        close(file[0]);
        close(file[1]);
        normal(tok[1]);
    }
}

void normal(char* command) {
    char temp[256];
    char* tok[256];
    int i = 1;

    strcpy(temp, command);

    tok[0] = strtok(temp, " ");
    while((tok[i] = strtok(NULL, " ")) != NULL) {
        i++;
    }

    execvp(tok[0], tok);
    exit(1);
}

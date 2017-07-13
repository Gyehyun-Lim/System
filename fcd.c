#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char dir[10][255];
int count = 0;

void fcd(char* filename, char* path) {
    struct dirent *dent;
	struct stat statbuf;
	char pathname[255];

    DIR *dp;

	if ((dp = opendir(path)) == NULL) {
		perror("opendir error");
		exit(1);
	}

	while((dent = readdir(dp)) != NULL) {
		sprintf(pathname, "%s/%s", path, dent->d_name);

        if((strcmp(dent->d_name, ".") == 0) || (strcmp(dent->d_name, "..")) == 0) {
            continue;
        }
		if (lstat(pathname, &statbuf) < 0) {
			perror("stat error");
			exit(1);
		}
		if(S_ISDIR(statbuf.st_mode)) {
            if(strcmp(dent->d_name, filename) == 0) {
                strcpy(dir[count++], pathname);
                //printf("dir : %s\n", dir[size]);
            }
            fcd(filename, pathname);
		}
	}
	closedir(dp);
}




int main(int argv, char* envp[]) {
    char filename[255];
    char buf[1024];

    printf("Enter the name of a directory : ");
    while(1) {
        scanf("%s", filename);
        if(strcmp(filename, ".") == 0) {
            printf("%s\n", buf);
            execl("/bin/bash", "", NULL);
            //break;
        }

        getcwd(buf, 1024);
        fcd(filename, buf);

        if(count == 0) {
            printf("There is no such directory\n");
            break;
        }
        else if(count == 1) {
            chdir(dir[0]);
            getcwd(buf, 1024);
            count = 0;
            printf("%s > ", buf);
        }
        else if(count > 1) {
            int i, j;
            for(i = 0; i < count; i++) {
                printf("[%d] %s\n", i + 1, dir[i]);
            }
            printf("Which directory do you want ? ");
            scanf("%d", &j);
            chdir(dir[j - 1]);
            getcwd(buf, 1024);
            count = 0;
            printf("%s > ", buf);
        }
    }

    return 0;
}

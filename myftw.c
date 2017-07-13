#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 255

void access_perm(char *perm, mode_t mode) {

	int i;
	char permchar[] = "rwx";
	memset(perm, '-', 10); perm[10] = '\0';
	if (S_ISDIR(mode))
		perm[0] = 'd';
	else if (S_ISCHR(mode))
		perm[0] = 'c';
	else if (S_ISBLK(mode))
		perm[0] = 'b';
	else if (S_ISFIFO(mode))
		perm[0] = 'p';
	else if (S_ISLNK(mode))
		perm[0] = 'l';
	for (i = 0; i < 9; i++){
		if ((mode << i) & 0x100)
			perm[i+1] = permchar[i%3];
		if (mode & S_ISUID)
			perm[3] = 's';
		if (mode & S_ISGID)
			perm[6] = 's';
		if (mode & S_ISVTX)
			perm[9] = 't';
	}
}

void myftw(char* filename) {
    struct dirent *dent;
	struct stat statbuf;
	char perm[11];
	char pathname[255];

    DIR *dp;

	if ((dp = opendir(filename)) == NULL) {
		perror("opendir error");
		exit(1);
	}

	printf("\nLists of Directory(%s): \n", filename);
	while((dent = readdir(dp)) != NULL) {
		sprintf(pathname, "%s/%s", filename, dent->d_name);
        if((strcmp(dent->d_name, ".") == 0) || (strcmp(dent->d_name, "..")) == 0) {
            continue;
        }
		if (lstat(pathname, &statbuf) < 0) {
			perror("stat error");
			exit(1);
		}
		if(!S_ISDIR(statbuf.st_mode)) {
			access_perm(perm, statbuf.st_mode);
            printf("%s %s\n", perm, dent->d_name);
		}
        else {
            access_perm(perm, statbuf.st_mode);
            printf("%s %s\n", perm, dent->d_name);
			myftw(pathname);
			printf("\n");
		}
	}
	closedir(dp);
}

int main(int argc, char* argv[]) {
    struct stat statbuf;
    char cur_dir[BUF_SIZE];

    if(argc < 2) {
        perror("Usage : ./myftw [filename]");
        exit(1);
    }

    if(access(argv[1], F_OK) == -1) {
        perror("access error");
        exit(1);
    }

    if(stat(argv[1], &statbuf) < 0) {
        perror("stat error");
        exit(1);
    }
    if(!S_ISDIR(statbuf.st_mode)) {
        fprintf(stderr, "%s is not directory\n", argv[1]);
        exit(1);
    }
    else{
        myftw(argv[1]);
    }

    return 0;
}

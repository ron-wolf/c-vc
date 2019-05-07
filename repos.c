#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "macros.h"

bool repos() {
    DIR * repos = opendir(".repos");
    return repos ? dirfd(repos) : -1;
}

void open_mutex_read(char * repo_name) {
    char * mutex_path = calloc(sizeof (char), (5 + 1 + strlen(repo_name) + 1 + 6) + 1);
    sprintf(mutex_path, ".repos/%s/.Mutex", repo_name);
    int mutex_fd = open(mutex_path, O_RDWR | O_CREAT | O_EXCL);
    if (mutex_fd < 0) {
        // the repo is already being worked on! SPPOOOOOKYYYY!!!!
    }
    
    FILE * mutex = fdopen(mutex_fd, "w");
    fprintf(mutex, "%d\tr\n", getpid());
    fclose(mutex);
}

void close_mutex_read(char * repo_name) {
    char * mutex_path = calloc(sizeof (char), (5 + 1 + strlen(repo_name) + 1 + 6) + 1);
    sprintf(mutex_path, ".repos/%s/.Mutex", repo_name);
    char * mutex_cmd = calloc(sizeof (char), ((2 + 1 + 2) + 1 + (5 + 1 + strlen(repo_name) + 1 + 6)) + 1);
    sprintf(mutex_cmd, "wc -l .repos/%s/.Mutex", mutex_path);
    
    FILE * mutex_lc = popen(mutex_cmd, "r");
    int lines; fscanf(mutex_lc, "%d\n", & lines); // assumes call to popen() worked
    if (lines <= 1) remove(mutex_path);
    else {
        // things get messy
    }
}

void checkout(char * repo_name, int sock_fd) {
    int repos_fd = repos();
    if (repos_fd < 0) EXIT(EX_CONFIG, "No repos are present on the system");
    
    int repo_fd = openat(repos_fd, repo_name, O_RDWR);
    if (repo_fd < 0) EXIT(EX_UNAVAILABLE, "No such repo \"%s\" available", repo_name);
    
    int man_fd = openat(repo_fd, ".Manifest", O_RDONLY);
    if (man_fd < 0) EXIT(EX_UNAVAILABLE, "Repo \"%s\" has no manifest file", repo_name);
    
    open_mutex_read(repo_name);
    
    char * cmd = calloc(sizeof (char), 65536);
    sprintf(cmd, "tar -cvf .repos/%s/repo.tar", repo_name);
    
    FILE * man_file = fdopen(man_fd, "r");
    char * filename = calloc(sizeof (char), 256);
    char * buf = calloc(sizeof (char), 256);
    while (fscanf(man_file, "%*lu\t%*u\t%[^\n]\n", filename) >= 1) {
        sprintf(buf, " --include \"%s\"", filename);
        cmd = strcat(cmd, buf);
        memset(buf, '\0', 256);
    }
    
    system(cmd);
    
    
    
    // write(sock_fd, "mn:");
    
    // FILE * sock_file = fdopen(sock_fd);
    
    close_mutex_read(repo_name);
}

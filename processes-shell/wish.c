#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char error_message[30] = "An error has occurred\n";

void print_error(){
    fwrite(error_message, strlen(error_message), 1, stderr);
}

int handle_builtin(char **args, char **paths, int *path_cnt){
    if(strcmp(args[0], "exit") == 0){
        if(args[1] != NULL){
            print_error();
            return 1;
        }
        return -1;
    }
    if(strcmp(args[0], "cd") == 0){
        if(args[1] == NULL || args[2] != NULL) print_error();
        else if(chdir(args[1]) != 0) print_error();
        return 1;
    }
    if(strcmp(args[0], "path") == 0){
        for(int i=0; i<*path_cnt; i++) free(paths[i]);
        *path_cnt = 0;
        for(int i=1; args[i] != NULL; i++){
            paths[(*path_cnt)++] = strdup(args[i]);
            if(*path_cnt == 63) break;
        }
        return 1;
    }
    return 0;
}

pid_t run_cmd(char **args, char **paths, int path_cnt, char *redir_file){
    char full_path[512];
    int found_executable = 0;
    for(int i=0; i<path_cnt; i++){
        snprintf(full_path, sizeof(full_path), "%s/%s", paths[i], args[0]);
        if(access(full_path, X_OK) == 0){
            found_executable = 1;
            break;
        }
    }
    if(!found_executable){
        print_error();
        return -1;
    }
    pid_t pid = fork();
    if(pid == 0){
        if (redir_file != NULL) {
            int fd = open(redir_file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if(fd<0){
                print_error();
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        execv(full_path, args);
        print_error();
        exit(1);
    }else if(pid > 0){
        return pid;
    }else{
        print_error();
        return -1;
    }
}

int main(int argc, char* argv[]){
    char *paths[64];
    paths[0] = strdup("/bin");
    int path_cnt = 1;
    FILE* in = stdin;

    if(argc == 2){
        in = fopen(argv[1], "r");
        if(in == NULL){
            print_error();
            exit(1);
        }
    }else if(argc > 2){
        print_error();
        exit(1);
    }

    while(1){
        char *line = NULL;
        size_t linecap = 0;
        ssize_t linelen;
        if(in == stdin){
            printf("wish> ");
            fflush(stdout);
        }
        linelen = getline(&line, &linecap, in);
        if(linelen == -1){
            free(line);
            break;
        }
        if(linelen > 0 && line[linelen-1] == '\n') line[linelen-1] = '\0';

        char *line_ptr = line;
        char *cmd_line;
        pid_t pids[64];
        int pid_cnt = 0;
        int exit_shell = 0;

        while((cmd_line = strsep(&line_ptr, "&")) != NULL){
            if(*cmd_line == '\0') continue;
            
            char *redir_str = cmd_line;
            char *cmd_part = strsep(&redir_str, ">");
            
            if(redir_str != NULL && strchr(redir_str, '>') != NULL){
                print_error();
                continue;
            }

            char *args[64];
            int arg_cnt = 0;
            char *token;
            while((token = strsep(&cmd_part, " \t\n")) != NULL){
                if(*token == '\0') continue;
                args[arg_cnt++] = token;
            }
            args[arg_cnt] = NULL;
            
            if(arg_cnt == 0){
                if (redir_str != NULL) print_error();
                continue;
            }

            char *redir_file = NULL;
            if(redir_str != NULL){
                char *redir_args[64];
                int r_arg_cnt = 0;
                while((token = strsep(&redir_str, " \t\n")) != NULL){
                    if(*token == '\0') continue;
                    redir_args[r_arg_cnt++] = token;
                }
                if(r_arg_cnt != 1){
                    print_error();
                    continue;
                }
                redir_file = redir_args[0];
            }

            int res = handle_builtin(args, paths, &path_cnt);
            if(res == -1){
                exit_shell = 1;
                break;
            }
            if(res == 0){
                pid_t pid = run_cmd(args, paths, path_cnt, redir_file);
                if(pid > 0) pids[pid_cnt++] = pid;
            }
        }
        for(int i=0; i<pid_cnt; i++) waitpid(pids[i], NULL, 0);
        free(line);
        if(exit_shell) break;
    }

    for(int i=0; i<path_cnt; i++) free(paths[i]);
    if(in != stdin) fclose(in);
    return 0;
}

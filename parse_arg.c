//parse_arg.c
#include <string.h> 

char * get_cmd (char * arg) {
    return strtok(arg, " ");
}
void get_arglist(char * arg, char ** list) {
    char *saveptr;
    int k = 0; 
    char *pch = strtok_r(arg," ",&saveptr);
    while (pch != NULL) {
        list[k++] = pch;
        pch = strtok_r(NULL, " ",&saveptr);
    }
    list[k++] = NULL;
}

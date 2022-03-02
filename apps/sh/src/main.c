#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

void showPrompt (void) {
    printf ("->");
}
int main(int argc, char *argv[]) {
    printf ("this is shell\n");
    showPrompt ();

    FILE   *fp = stdin;
    char   *line = NULL;
    size_t  len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != EOF) 
    {
        line[read-1] = 0;

        if(strlen(line))
        {
            printf ("input is: %s", line);
        }
        showPrompt();
    }

    return 0;
}



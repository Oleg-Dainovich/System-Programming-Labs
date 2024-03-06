#include <stdio.h>
#include <string.h>
#include "symbol_inversion.h"

#define MAX_LENGTH 1000

int main() {
    char line[MAX_LENGTH];
    FILE* file = fopen("test.txt", "r");

    if (file == NULL) {
        printf("Error. Failed to open file.\n");
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        symbolInversion(line);
        printf("%s\n", line);
    }

    fclose(file);
    return 0;
}

#include "symbol_inversion.h"
#include <string.h>

void symbolInversion(char* str) {
    int length = strlen(str);
    int i = 0;
    int j = length - 1;

    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

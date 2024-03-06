#include <stdio.h>
#include <string.h>
#include "symbol_inversion.h"

#define MAX_LENGTH 1000

void testSymbolInversion(const char* input, const char* expected) {
    char str[MAX_LENGTH];
    strcpy(str, input);
    
    symbolInversion(str);
    
    if (strcmp(str, expected) == 0) {
        printf("PASSED: Input: \"%s\", Expected: \"%s\", Result: \"%s\"\n", input, expected, str);
    } else {
        printf("FAILED: Input: \"%s\", Expected: \"%s\", Result: \"%s\"\n", input, expected, str);
    }
}

int main() {
    testSymbolInversion("", "");
    testSymbolInversion("Hello", "olleH");
    testSymbolInversion("12345", "54321");
    testSymbolInversion("qwerty", "qwerty");

    return 0;
}

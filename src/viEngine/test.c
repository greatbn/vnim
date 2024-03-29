#include <stdio.h>
#include <locale.h> 
#include "vi-engine.h"

int main(int argc, char **argv) {
    setlocale(LC_ALL, "en_US.UTF-8");
    ViInitEngine();
    
    wchar_t resultText[100];
    int resultLength;
    UChar keyLists[] = "nguyeenx";
    int i, retVal;
    for (i = 0; i < strlen(keyLists); i++) {
        retVal = ViProcessKey(keyLists[i], keyLists[i] == toupper(keyLists[i]));
        if (retVal != VNTrue) {
            printf("Word end\n");
            break;
        }
    }
    ViGetCurrentWord(resultText, &resultLength);
    printf("resultLength == %d\n",wcslen(resultText));

    fwide(stdout, 0);
    fwprintf(stderr,L"Result: %ls\n",resultText);
    
    ViDestroyEngine();
}
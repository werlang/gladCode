#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gladCodeRuntimeRender.c"

struct params {
    char code[50];
    int id;
    int n;
};

DWORD WINAPI thread(LPVOID p){
    struct params *input = p;

    printf("thread started received %s %i %i\n",input->code,input->id, input->n);

    char call[100];
    sprintf(call, "gcc -o %s.exe %s", input->code, input->code);
    printf("Compiling %s...\n",input->code);
    system(call);
    printf("%s compilled.\n",input->code);
    sprintf(call, "%s.exe %i %i",input->code, input->id, input->n);
    printf("Running %s.exe...\n",input->code);
    system(call);
    printf("%s.exe stopped.\n",input->code);
    return 0;
}

int main(){
    DWORD dwThreadId;
    HANDLE hThread;

    FILE *arq;
    arq = fopen("struct", "rb");
    if (arq != NULL){
        fclose(arq);
        system("del struct");
    }
    arq = fopen("lock", "rb");
    if (arq != NULL){
        fclose(arq);
        system("del lock");
    }
    arq = fopen("output.txt", "w");
    fclose(arq);
    int n;
    scanf("%i",&n);
    struct params p[n];
    int i;
    for (i=0 ; i<n ; i++){
        char input[100];
        scanf("%s",p[i].code);
        p[i].id = i;
        p[i].n = n;

        hThread = CreateThread(
            NULL, // default security attributes
            0, // use default stack size
            thread, // thread function
            &p[i], // argument to thread function
            0, // use default creation flags
            &dwThreadId); // returns the thread identifier

        // Check the return value for success. If something wrong...
        if (hThread == NULL)
            printf("CreateThread() failed, error: %d.\n", GetLastError());
    }

    printf("Starting render...\n");
    if (!renderInit(n)){
        return -1;
    }
    printf("Render started.\n");
    renderLoop();


    arq = fopen("struct", "rb");
    if (arq != NULL){
        fclose(arq);
        system("del struct");
    }
    arq = fopen("lock", "rb");
    if (arq != NULL){
        fclose(arq);
        system("del lock");
    }
    return 0;
}

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#pragma comment(lib, "user32.lib")

#define BUF_SIZE 256
char mtName[50]="MyMutexObject";
char fmName[50]="StructFileMapObject";
char ctName[50]="CounterFileMapObject";
HANDLE ghMutex;

//funcao que inicia a memoria compartilhada entre os processos
int startSharedMemory(int size, char *name){
    HANDLE hMapFile;

    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        size,                // maximum object size (low-order DWORD)
        name);                 // name of mapping object

    if (hMapFile == NULL)
        return 0;
    else
        return 1;
}

//cria memoria compartilhada do tamanho da struct dos gladiadores
void startStructSharedMemory(){
    if (startSharedMemory(sizeof(struct gladiador) * nglad, fmName))
        printf("Struct filemap created.\n");
    else
        printf("Could not create file mapping object (%d).\n", GetLastError());
}

//cria memoria compartilhada para o tempo trnascorrido por cada processo
void startCounterSharedMemory(){
    if (startSharedMemory(sizeof(double) * nglad, ctName))
        printf("Simtime filemap created.\n");
    else
        printf("Could not create file mapping object (%d).\n", GetLastError());
}

void writeOnSharedMemory(void *info, int size){
    HANDLE hMapFile;
    void *pBuf;

    //acessa a memoria pelo handler
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        fmName);               // name of mapping object

    if (hMapFile == NULL){
        _tprintf("Could not open file mapping object (%d).\n",GetLastError());
        return;
    }

    //recebe o conteudo da memoria em pBuf
    pBuf = (void*) MapViewOfFile(hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        0);

    if (pBuf == NULL){
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    }

    CopyMemory((void*)pBuf, info, size);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    return;
}

//atualiza o contador de tempo da simulacao de um gladiador
void updateSimCounter(int i, float timeInterval){
    waitForMutex();

    HANDLE hMapFile;
    double *pBuf;

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        ctName);               // name of mapping object

    if (hMapFile == NULL){
        printf("Could not open file mapping object (%d).\n",GetLastError());
        return;
    }

    pBuf = (double*) MapViewOfFile(hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        0);

    if (pBuf == NULL){
        _tprintf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    }

    double *cont = (double*)malloc(sizeof(double) * nglad);
    memcpy(cont, pBuf, sizeof(double) * nglad);
    *(cont + i) += timeInterval;
    CopyMemory((double*)pBuf, cont, sizeof(double) * nglad);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    free(cont);

    releaseMutex();
}

//retorna o tempo atual de um processo
double getSimCounter(int i){
    HANDLE hMapFile;
    double *pBuf;

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        ctName);               // name of mapping object

    if (hMapFile == NULL){
        printf("Could not open file mapping object (%d).\n",GetLastError());
        return;
    }

    pBuf = (double*) MapViewOfFile(hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        0);

    if (pBuf == NULL){
        _tprintf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    }

    double *cont = (double*)malloc(sizeof(double) * nglad);
    memcpy(cont, pBuf, sizeof(double) * nglad);
    double n = *(cont + i);
    free(cont);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return n;
}

//atribui por referencia no vetor o contador de tempo de todos gladiadores
void getAllSimCounters(double cont[]){
    HANDLE hMapFile;
    double *pBuf;

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        ctName);               // name of mapping object

    if (hMapFile == NULL){
        printf("Could not open file mapping object (%d).\n",GetLastError());
        return;
    }

    pBuf = (double*) MapViewOfFile(hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        0);

    if (pBuf == NULL){
        _tprintf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    }

    memcpy(cont, pBuf, sizeof(double) * nglad);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
}

//le e retorna um ponteiro para a memoria compartilhada
void *readFromSharedMemory(int size){
    HANDLE hMapFile;
    void *pBuf, *result;

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        fmName);               // name of mapping object

    if (hMapFile == NULL){
        printf("Could not open file mapping object (%d).\n",GetLastError());
        return;
    }

    pBuf = (void*) MapViewOfFile(hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        0);

    if (pBuf == NULL){
        _tprintf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    }

    result = malloc(size);
    memcpy ( result, pBuf, size );

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return result;
}

void loadStructFromMemory(){
    int size = sizeof(struct gladiador) * nglad;
    if (g != NULL){
        free(g);
    }
    g = (struct gladiador*)readFromSharedMemory(size);
}

void saveStructToMemory(){
    int size = sizeof(struct gladiador) * nglad;
    writeOnSharedMemory(g, size);
}

//inicia a mutex, que garante que n�o mais que um processo vai estar escrevendo ao mesmo tempo na memoria compartilhada
void startMutex(){
    ghMutex = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        mtName);             // mutex name

    if (ghMutex == NULL)
        printf("CreateMutex error: %d\n", (int)GetLastError());
    else
        printf("Mutex on %i started\n", gladid);
}

int waitForMutex(){
    DWORD dwWaitResult;

    dwWaitResult = WaitForSingleObject(
        ghMutex,    // handle to mutex
        INFINITE);  // no time-out interval

    if (dwWaitResult == WAIT_OBJECT_0){
        return 1;
        //printf("Thread %d writing to database...\n", GetCurrentThreadId());
    }
    else if (dwWaitResult == WAIT_ABANDONED){
        printf("Error waiting for Mutex.\n");
        return 0;
    }
}

int releaseMutex(){
    if (!ReleaseMutex(ghMutex)){
        printf("Error releasing Mutex.\n");
        return 0;
    }
    return 1;
}

void restartSharedMemory(){
    HANDLE hMapFile;
    double *pBuf;

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        ctName);               // name of mapping object

    if (hMapFile == NULL){
        printf("Could not open file mapping object (%d).\n",GetLastError());
        return;
    }

    pBuf = (double*) MapViewOfFile(hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        0);

    if (pBuf == NULL){
        _tprintf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    }

    int i;
    double *cont = (double*)malloc(sizeof(double) * nglad);
    for (i=0 ; i<nglad ; i++){
        *(cont + i) = 0;
    }
    CopyMemory((double*)pBuf, cont, sizeof(double) * nglad);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    free(cont);


    //acessa a memoria pelo handler
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        fmName);               // name of mapping object

    if (hMapFile == NULL){
        _tprintf("Could not open file mapping object (%d).\n",GetLastError());
        return;
    }

    //recebe o conteudo da memoria em pBuf
    pBuf = (void*) MapViewOfFile(hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        0);

    if (pBuf == NULL){
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    }

    int size = sizeof(struct gladiador) * nglad;
    void *result = malloc(size);
    memcpy ( result, pBuf, size );

    g = (struct gladiador*)result;
    for (i=0 ; i<nglad ; i++)
        (g+i)->lvl = 0;

    CopyMemory((void*)pBuf, g, size);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

}


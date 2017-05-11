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

void startStructSharedMemory(){
    if (startSharedMemory(sizeof(struct gladiador) * nglad, fmName))
        printf("Struct filemap created.\n");
    else
        printf("Could not create file mapping object (%d).\n", GetLastError());
}

void startCounterSharedMemory(){
    if (startSharedMemory(sizeof(double) * nglad, ctName))
        printf("Simtime filemap created.\n");
    else
        printf("Could not create file mapping object (%d).\n", GetLastError());
}

void writeOnSharedMemory(void *info, int size){
    HANDLE hMapFile;
    void *pBuf;

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        fmName);               // name of mapping object

    if (hMapFile == NULL){
        _tprintf("Could not open file mapping object (%d).\n",GetLastError());
        return;
    }

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

void updateSimCounter(int i, float timeInterval){
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
}

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

void *readFromSharedMemory(int size){
    HANDLE hMapFile;
    void *pBuf, *result;

    //printf("%i 0\n",gladid);

    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        fmName);               // name of mapping object

    //printf("%i 1\n",gladid);
    if (hMapFile == NULL){
        printf("Could not open file mapping object (%d).\n",GetLastError());
        return;
    }

    pBuf = (void*) MapViewOfFile(hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        0);

    //printf("%i 2\n",gladid);
    if (pBuf == NULL){
        _tprintf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return;
    }

    result = malloc(size);
    memcpy ( result, pBuf, size );
    //printf("%i 3\n",gladid);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return result;
}

void loadStructFromMemory(){
    int size = sizeof(struct gladiador) * nglad;
    //printf("%i vumve\n",gladid);
    if (g != NULL){
        //printf("%i nao null\n",gladid);
        free(g);
        //printf("%i freezou\n",gladid);
    }
    //printf("%i fora do if\n",gladid);
    g = (struct gladiador*)readFromSharedMemory(size);
}

void saveStructToMemory(){
    int size = sizeof(struct gladiador) * nglad;
    writeOnSharedMemory(g, size);
}

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

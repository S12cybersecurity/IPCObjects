#include <Windows.h>
#include <iostream>
#include "Serialitzator.h"

using namespace std;

int main(){
    HANDLE currentProcess = GetCurrentProcess();
    Serialitzator serialitzator("ipcObject");
    serialitzator.serializeHandle(currentProcess);
    HANDLE deserializedHandle = serialitzator.deserialitzateHandle("ipcObject");

    DWORD pid = GetProcessId(deserializedHandle);
    cout << "PID: " << pid << endl;
    getchar();
}

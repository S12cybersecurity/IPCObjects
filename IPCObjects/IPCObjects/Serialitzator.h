#include <Windows.h>
#include <iostream>

using namespace std;

class Serialitzator {
	private:
		string fileName;
	public:
		// Constructor
		Serialitzator(string fileName) {
			this->fileName = fileName;
		}

		// Getters
		string getFileName() {
			return this->fileName;
		}
		// Setters
		void setFileName(string fileName) {
			this->fileName = fileName;
		}
		
		// Methods
		bool serializeHandle(HANDLE targetHandle) {
			HANDLE mappedFile = NULL;
			LPVOID mappedView;
			mappedFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(targetHandle), "ipcObject");
			if (mappedFile == NULL) {
				cout << "Error creating the mapped file";
				return false;
			}
			
			mappedView = MapViewOfFile(mappedFile, FILE_MAP_WRITE, 0, 0, sizeof(targetHandle));
			if (mappedView == NULL) {
				cout << "Error creating the view";
				return false;
			}

			RtlMoveMemory(mappedView, &targetHandle, sizeof(targetHandle));
			UnmapViewOfFile(mappedView);
			return true;
		}

		HANDLE deserialitzateHandle(string mappedFileName) {
			HANDLE mappedFile = NULL;
			LPVOID mappedView;
			mappedFile = OpenFileMappingA(FILE_MAP_WRITE, FALSE, mappedFileName.c_str());
			if (mappedFile == NULL) {
				cout << "Error opening the Mapped File";
				return mappedFile;
			}

			mappedView = MapViewOfFile(mappedFile, FILE_MAP_WRITE, 0, 0, sizeof(mappedFile));
			if (mappedView == NULL) {
				cout << "Error creating the view";
				return mappedFile;
			}

			HANDLE targetHandle;
			RtlMoveMemory(&targetHandle, mappedView, sizeof(mappedFile));
			UnmapViewOfFile(mappedView);
			return targetHandle;
		}
			

};
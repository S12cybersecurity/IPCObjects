#include <Windows.h>
#include <unordered_map>
#include <vector>
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

	bool serializeHashMap(unordered_map<string, vector<string>> targetHashMap, LPVOID mappedFileView) {
		// Serialize the size of the hashmap
		int hashMapSize = targetHashMap.size();
		RtlMoveMemory(mappedFileView, &hashMapSize, sizeof(hashMapSize));
		mappedFileView = (LPVOID)((DWORD)mappedFileView + sizeof(hashMapSize));

		// Serialize the hashmap
		for (auto it = targetHashMap.begin(); it != targetHashMap.end(); it++) {
			// Serialize the key
			int keySize = it->first.size();
			RtlMoveMemory(mappedFileView, &keySize, sizeof(keySize));
			mappedFileView = (LPVOID)((DWORD)mappedFileView + sizeof(keySize));
			RtlMoveMemory(mappedFileView, it->first.c_str(), keySize * sizeof(wchar_t));
			mappedFileView = (LPVOID)((DWORD)mappedFileView + keySize * sizeof(wchar_t));

			// Serialize the vector
			int vectorSize = it->second.size();
			RtlMoveMemory(mappedFileView, &vectorSize, sizeof(vectorSize));
			mappedFileView = (LPVOID)((DWORD)mappedFileView + sizeof(vectorSize));
			for (int i = 0; i < vectorSize; i++) {
				int stringSize = it->second[i].size();
				RtlMoveMemory(mappedFileView, &stringSize, sizeof(stringSize));
				mappedFileView = (LPVOID)((DWORD)mappedFileView + sizeof(stringSize));
				RtlMoveMemory(mappedFileView, it->second[i].c_str(), stringSize * sizeof(wchar_t));
				mappedFileView = (LPVOID)((DWORD)mappedFileView + stringSize * sizeof(wchar_t));
			}
		}
		return true;
	}


	unordered_map<string, vector<string>> deserialitzateHashMap(string mappedFileName) {
		HANDLE mappedFile = OpenFileMappingA(FILE_MAP_READ, FALSE, mappedFileName.c_str());
		if (mappedFile == NULL) {
			cout << "Error opening the Mapped File";
			return unordered_map<string, vector<string>>();
		}

		LPVOID mappedView = MapViewOfFile(mappedFile, FILE_MAP_READ, 0, 0, 0);
		if (mappedView == NULL) {
			cout << "Error creating the view";
			CloseHandle(mappedFile);
			return unordered_map<string, vector<string>>();
		}

		unordered_map<string, vector<string>> targetHashMap;
		// Deserialize the size of the hashmap
		int hashMapSize;
		RtlMoveMemory(&hashMapSize, mappedView, sizeof(hashMapSize));
		mappedView = (LPVOID)((DWORD)mappedView + sizeof(hashMapSize));

		// Deserialize the hashmap
		for (int i = 0; i < hashMapSize; i++) {
			// Deserialize the key
			int keySize;
			RtlMoveMemory(&keySize, mappedView, sizeof(keySize));
			mappedView = (LPVOID)((DWORD)mappedView + sizeof(keySize));
			char* key = new char[keySize + 1];
			RtlMoveMemory(key, mappedView, keySize);
			key[keySize] = '\0';  // Null-terminate the key
			mappedView = (LPVOID)((DWORD)mappedView + keySize);

			// Deserialize the vector
			int vectorSize;
			RtlMoveMemory(&vectorSize, mappedView, sizeof(vectorSize));
			mappedView = (LPVOID)((DWORD)mappedView + sizeof(vectorSize));
			vector<string> targetVector;
			for (int j = 0; j < vectorSize; j++) {
				int stringSize;
				RtlMoveMemory(&stringSize, mappedView, sizeof(stringSize));
				mappedView = (LPVOID)((DWORD)mappedView + sizeof(stringSize));
				char* str = new char[stringSize + 1];
				RtlMoveMemory(str, mappedView, stringSize);
				str[stringSize] = '\0';  // Null-terminate the string
				mappedView = (LPVOID)((DWORD)mappedView + stringSize);
				targetVector.push_back(str);
				delete[] str;  // Free the allocated memory
			}
			targetHashMap[key] = targetVector;
			delete[] key;  // Free the allocated memory
		}

		CloseHandle(mappedFile);
		return targetHashMap;
	}

};

#include <Windows.h>
#include <iostream>

using namespace std;

class MappedFiles {
	private:
		string fileName;
	public:
		// Constructor
		MappedFiles(string fileName) {
			this->fileName = fileName;
		}

		// Getter
		string getFileName() {
			return this->fileName;
		}

		void setFileName(string fileName) {
			this->fileName = fileName;
		}
};
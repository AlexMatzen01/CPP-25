/*
#FileEncryptionFilter accepts no arguments
#it encrypts or decrypts text/binary file contents using a simple byte-shift cipher
#it outputs a new file containing either encrypted bytes or restored original bytes
*/

#include <fstream>
#include <iostream>
#include <limits>
#include <string>

using namespace std;

namespace {
const int SHIFT_VALUE = 10;

void clearInputState() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readMenuChoice() {
    int choice = 0;

    while (true) {
        cout << "\nFile Encryption Filter\n";
        cout << "1. Encrypt a file\n";
        cout << "2. Decrypt a file\n";
        cout << "3. Exit\n";
        cout << "Choose an option (1-3): ";

        if (cin >> choice && choice >= 1 && choice <= 3) {
            clearInputState();
            return choice;
        }

        cout << "Invalid choice. Please enter 1, 2, or 3.\n";
        clearInputState();
    }
}

bool transformFile(const string& sourcePath, const string& destinationPath, int shiftDelta) {
    ifstream inputFile(sourcePath, ios::binary);
    if (!inputFile) {
        cout << "Could not open source file: " << sourcePath << "\n";
        return false;
    }

    ofstream outputFile(destinationPath, ios::binary);
    if (!outputFile) {
        cout << "Could not open destination file: " << destinationPath << "\n";
        return false;
    }

    char currentByte = '\0';
    while (inputFile.get(currentByte)) {
        unsigned char byteValue = static_cast<unsigned char>(currentByte);
        unsigned char transformedValue = static_cast<unsigned char>(byteValue + shiftDelta);
        outputFile.put(static_cast<char>(transformedValue));
    }

    if (!inputFile.eof()) {
        cout << "An error occurred while reading from source file.\n";
        return false;
    }

    if (!outputFile) {
        cout << "An error occurred while writing to destination file.\n";
        return false;
    }

    return true;
}

void processEncryption() {
    string sourcePath;
    string destinationPath;

    cout << "Enter the path of the file to encrypt: ";
    getline(cin, sourcePath);
    cout << "Enter the output encrypted file path: ";
    getline(cin, destinationPath);

    if (transformFile(sourcePath, destinationPath, SHIFT_VALUE)) {
        cout << "Encryption complete. Output written to: " << destinationPath << "\n";
    }
}

void processDecryption() {
    string sourcePath;
    string destinationPath;

    cout << "Enter the path of the file to decrypt: ";
    getline(cin, sourcePath);
    cout << "Enter the output decrypted file path: ";
    getline(cin, destinationPath);

    if (transformFile(sourcePath, destinationPath, -SHIFT_VALUE)) {
        cout << "Decryption complete. Output written to: " << destinationPath << "\n";
    }
}
}  // namespace

int main() {
    while (true) {
        const int choice = readMenuChoice();

        if (choice == 1) {
            processEncryption();
        } else if (choice == 2) {
            processDecryption();
        } else {
            cout << "Program ended.\n";
            break;
        }
    }

    return 0;
}

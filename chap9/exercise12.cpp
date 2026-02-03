#include <iostream>

using namespace std;

int* shiftElements(int *arr, int size);
void displayArray(int *arr, int size);

int main() {
    int size = 5;
    int original[] = {1, 2, 3, 4, 5};

    cout << "Original Array: ";
    displayArray(original, size);

    int *shifted = shiftElements(original, size);

    cout << "Shifted Array:  ";
    displayArray(shifted, size + 1);

    // Free memory
    delete[] shifted;

    return 0;
}

int* shiftElements(int *arr, int size) {
    if (size < 0) return nullptr;

    // Create a new array that is one element larger
    int *newArr = new int[size + 1];

    // The first element of the new array should be set to 0.
    *newArr = 0;

    // Copy elements from the original array to the new one, shifted by 1
    for (int i = 0; i < size; i++) {
        *(newArr + i + 1) = *(arr + i);
    }

    return newArr;
}

void displayArray(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        cout << *(arr + i) << " ";
    }
    cout << endl;
}

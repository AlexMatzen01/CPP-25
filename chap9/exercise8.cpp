#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int findMode(int *arr, int size);

int main() {
    int testCase1[] = {1, 2, 2, 3, 4};
    int testCase2[] = {10, 20, 30, 40};
    int testCase3[] = {5, 5, 8, 8, 8, 2, 1};

    cout << "Mode of {1, 2, 2, 3, 4}: " << findMode(testCase1, 5) << " (Expected: 2)" << endl;
    cout << "Mode of {10, 20, 30, 40}: " << findMode(testCase2, 4) << " (Expected: -1)" << endl;
    cout << "Mode of {5, 5, 8, 8, 8, 2, 1}: " << findMode(testCase3, 7) << " (Expected: 8)" << endl;

    return 0;
}

int findMode(int *arr, int size) {
    if (size <= 0) return -1;

    // Sorting helps in counting frequencies easily
    // Create a copy to avoid modifying original array if desired, 
    // but the exercise doesn't forbid sorting. 
    // Using std::sort on the array pointer range.
    sort(arr, arr + size);

    int mode = -1;
    int maxFreq = 1;
    int currentFreq = 1;
    bool modeFound = false;

    for (int i = 1; i < size; i++) {
        if (*(arr + i) == *(arr + (i - 1))) {
            currentFreq++;
        } else {
            if (currentFreq > maxFreq) {
                maxFreq = currentFreq;
                mode = *(arr + (i - 1));
                modeFound = true;
            }
            currentFreq = 1;
        }
    }

    // Final check for the last group
    if (currentFreq > maxFreq) {
        maxFreq = currentFreq;
        mode = *(arr + (size - 1));
        modeFound = true;
    }

    return modeFound ? mode : -1;
}

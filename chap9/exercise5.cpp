#include <iostream>

using namespace std;

// Original: int doSomething(int &x, int &y)
// Pointer rewrite:
int doSomething(int *x, int *y)
{
	int temp = *x;
	*x = *y * 10;
	*y = temp * 10;
	return *x + *y;
}

int main() {
    int a = 5;
    int b = 10;

    cout << "Before calling doSomething:\n";
    cout << "a = " << a << ", b = " << b << endl;

    int result = doSomething(&a, &b);

    cout << "\nAfter calling doSomething:\n";
    cout << "a = " << a << ", b = " << b << endl;
    cout << "Result (a + b): " << result << endl;

    return 0;
}

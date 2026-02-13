#include <NTL/ZZ.h>
#include <iostream>
#include <string>
#include "headers/BigInt.h"

using namespace NTL;
using namespace std;

int main() {
	ZZ a, b;
	cout << "Enter first integer: ";
	string input;
	getline(cin, input);
	a = ZZ(INIT_VAL, input.c_str());
	cout << "Enter second integer: ";
	getline(cin, input);
	b = ZZ(INIT_VAL, input.c_str());
	ZZ x, y;
	ZZ gcd = BigInt::ExtendedGCD(a, b, x, y);
	cout << "GCD: " << gcd << endl;
	cout << "Coefficients x and y such that ax + by = GCD(a, b):" << endl;
	cout << "x: " << x << endl;
	cout << "y: " << y << endl;

    return 0;
}

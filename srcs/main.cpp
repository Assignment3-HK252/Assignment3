#include <NTL/ZZ.h>
#include <iostream>
#include <string>
#include "headers/BigInt.h"

using namespace NTL;
using namespace std;

int main() {
	
	cout << BigInt::GenerateLargePrime(1024) << std::endl;

    return 0;
}

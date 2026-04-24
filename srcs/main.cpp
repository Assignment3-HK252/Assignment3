#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include <NTL/ZZ.h>
#include "headers/BigInt.h"

using namespace std;

// ============================================================
//  Exception dùng để hủy lựa chọn hiện tại
//  Người dùng gõ 'q' hoặc 'Q' tại bất kỳ prompt nào để kích hoạt
// ============================================================
struct CancelException : public exception {
    const char* what() const noexcept override {
        return "Nguoi dung huy thao tac.";
    }
};

// Ném khi người dùng gõ 'e'/'E' → thoát chương trình ngay lập tức
struct ExitException : public exception {
    const char* what() const noexcept override {
        return "Nguoi dung yeu cau thoat.";
    }
};

// ============================================================
//  Tiện ích: chuyển string <-> ZZ (encode từng byte)
//  Lưu ý: chỉ dùng cho demo với chuỗi ngắn (message < n)
// ============================================================
NTL::ZZ StringToZZ(const string& s) {
    NTL::ZZ result(0);
    for (unsigned char c : s) {
        result = result * 256 + NTL::ZZ(c);
    }
    return result;
}

string ZZToString(NTL::ZZ n) {
    if (n == 0) return string(1, '\0');
    string result;
    while (n > 0) {
        long byte_val = NTL::to_long(n % 256);
        result = char(byte_val) + result;
        n /= 256;
    }
    return result;
}

// ============================================================
//  In đường phân cách
// ============================================================
void printSep(const string& title = "") {
    cout << "\n" << string(60, '=');
    if (!title.empty()) cout << "\n  " << title;
    cout << "\n" << string(60, '=') << "\n";
}

// ============================================================
//  Tiện ích nhập liệu  –  gõ 'q' / 'Q' để hủy bất kỳ lúc nào
// ============================================================

// Kiểm tra chuỗi có phải lệnh hủy / thoát không
inline bool isCancel(const string& s) { return s == "q" || s == "Q"; }
inline bool isExit(const string& s) { return s == "e" || s == "E"; }

// Kiểm tra và ném exception tương ứng nếu cần
inline void checkSpecial(const string& s) {
    if (isExit(s))   throw ExitException{};
    if (isCancel(s)) throw CancelException{};
}

// Đọc số nguyên lớn (ZZ). Gõ 'q' = hủy, 'e' = thoát
NTL::ZZ readZZ(const string& prompt) {
    while (true) {
        cout << prompt << " (q=huy / e=thoat) ";
        string s;
        cin >> s;
        checkSpecial(s);

        bool valid = !s.empty();
        size_t start = (s[0] == '-') ? 1 : 0;
        for (size_t i = start; i < s.size(); i++) {
            if (!isdigit((unsigned char)s[i])) { valid = false; break; }
        }
        if (valid) {
            NTL::ZZ val;
            NTL::conv(val, s.c_str());
            return val;
        }
        cout << "  [Loi] Chi nhap so nguyen, 'q' de huy, 'e' de thoat. Thu lai.\n";
    }
}

// Đọc số nguyên thông thường trong [lo, hi]. Gõ 'q' = hủy, 'e' = thoát
int readInt(const string& prompt, int lo, int hi) {
    while (true) {
        cout << prompt << " (q=huy / e=thoat) ";
        string s;
        cin >> s;
        checkSpecial(s);

        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size() && v >= lo && v <= hi) return v;
        }
        catch (...) {}
        cout << "  [Loi] Nhap so nguyen trong khoang [" << lo << ", " << hi
            << "], 'q' de huy, 'e' de thoat. Thu lai.\n";
    }
}

// Đọc một dòng chuỗi. Gõ 'q' = hủy, 'e' = thoát
string readLine(const string& prompt) {
    cout << prompt << " (q=huy / e=thoat) ";
    if (cin.peek() == '\n') cin.ignore();
    string s;
    getline(cin, s);
    checkSpecial(s);
    return s;
}

// ============================================================
//  Demo 1: Kiểm tra các hàm toán học cơ bản
// ============================================================
void demo_math() {
    printSep("DEMO 1: Cac ham toan hoc co ban");

    NTL::ZZ a = NTL::ZZ(252), b = NTL::ZZ(105);

    cout << "GCD(" << a << ", " << b << ") = "
        << BigInt::GCD(a, b) << "\n";

    NTL::ZZ x, y;
    NTL::ZZ g = BigInt::ExtendedGCD(a, b, x, y);
    cout << "ExtendedGCD: gcd=" << g
        << ",  x=" << x << ",  y=" << y << "\n";
    cout << "  Kiem tra: " << a << "*" << x
        << " + " << b << "*" << y << " = " << (a * x + b * y) << "\n";

    NTL::ZZ A = NTL::ZZ(3), M = NTL::ZZ(11);
    NTL::ZZ inv = BigInt::ModularInverse(A, M);
    cout << "ModularInverse(" << A << ", " << M << ") = " << inv << "\n";
    cout << "  Kiem tra: " << A << " * " << inv
        << " mod " << M << " = " << (A * inv) % M << "\n";

    NTL::ZZ base = NTL::ZZ(2), exp = NTL::ZZ(10), mod = NTL::ZZ(1000);
    cout << "PowerMod(" << base << "^" << exp
        << " mod " << mod << ") = "
        << BigInt::PowerMod(base, exp, mod) << "\n";
}

// ============================================================
//  Demo 2: Sinh số nguyên tố lớn
// ============================================================
void demo_prime() {
    printSep("DEMO 2: Sinh so nguyen to lon");

    cout << "Dang sinh so nguyen to 512-bit...\n";
    NTL::ZZ p512 = BigInt::GenerateStrongLargePrime(512);
    cout << "p (512-bit) = " << p512 << "\n";
    cout << "So bit thuc te: " << NTL::NumBits(p512) << "\n";

    cout << "\nDang sinh so nguyen to 1024-bit...\n";
    NTL::ZZ p1024 = BigInt::GenerateStrongLargePrime(1024);
    cout << "p (1024-bit) = " << p1024 << "\n";
    cout << "So bit thuc te: " << NTL::NumBits(p1024) << "\n";
}

// ============================================================
//  Demo 3: Tạo khóa RSA
// ============================================================
void demo_keygen(int bits = 512) {
    printSep("DEMO 3: Tao bo khoa RSA (" + to_string(bits * 2) + "-bit modulus)");

    cout << "Dang sinh p (" << bits << "-bit)...\n";
    NTL::ZZ p = BigInt::GenerateStrongLargePrime(bits);

    cout << "Dang sinh q (" << bits << "-bit)...\n";
    NTL::ZZ q = BigInt::GenerateStrongLargePrime(bits);

    while (p == q) {
        cout << "p == q, sinh lai q...\n";
        q = BigInt::GenerateStrongLargePrime(bits);
    }

    cout << "\np = " << p << "\n";
    cout << "q = " << q << "\n";

    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);

    cout << "\n--- Bo khoa RSA ---\n";
    cout << "n   = " << kp.n << "\n";
    cout << "e   = " << kp.e << "\n";
    cout << "d   = " << kp.d << "\n";
    cout << "\nSo bit cua n: " << NTL::NumBits(kp.n) << "\n";

    NTL::ZZ phi = (p - 1) * (q - 1);
    NTL::ZZ check = (kp.e * kp.d) % phi;
    cout << "Kiem tra: e*d mod phi(n) = " << check
        << (check == 1 ? "  [OK]" : "  [FAILED]") << "\n";
}

// ============================================================
//  Demo 4: Mã hóa / Giải mã số nguyên
// ============================================================
void demo_encrypt_number() {
    printSep("DEMO 4: Ma hoa / Giai ma so nguyen (512-bit keys)");

    NTL::ZZ p = BigInt::GenerateStrongLargePrime(512);
    NTL::ZZ q = BigInt::GenerateStrongLargePrime(512);
    while (p == q) q = BigInt::GenerateStrongLargePrime(512);

    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);

    NTL::ZZ message = NTL::ZZ(123456789);
    cout << "Thong diep goc (M)       : " << message << "\n";

    NTL::ZZ cipher = BigInt::Encrypt(message, kp.e, kp.n);
    cout << "Ban ma (C = M^e mod n)   : " << cipher << "\n";

    NTL::ZZ decrypted = BigInt::Decrypt(cipher, kp.d, kp.n);
    cout << "Giai ma (M = C^d mod n)  : " << decrypted << "\n";

    cout << "Ket qua: "
        << (message == decrypted ? "THANH CONG [OK]" : "THAT BAI [FAILED]")
        << "\n";
}

// ============================================================
//  Demo 5: Mã hóa / Giải mã chuỗi văn bản
// ============================================================
void demo_encrypt_string() {
    printSep("DEMO 5: Ma hoa / Giai ma chuoi van ban");

    NTL::ZZ p = BigInt::GenerateStrongLargePrime(512);
    NTL::ZZ q = BigInt::GenerateStrongLargePrime(512);
    while (p == q) q = BigInt::GenerateStrongLargePrime(512);

    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);

    string plaintext = "Hello RSA!";
    cout << "Plaintext  : \"" << plaintext << "\"\n";

    NTL::ZZ M = StringToZZ(plaintext);
    cout << "M (so)     : " << M << "\n";

    if (M >= kp.n) {
        cout << "[LOI] Message qua lon so voi n. Chon chuoi ngan hon hoac tang bit.\n";
        return;
    }

    NTL::ZZ C = BigInt::Encrypt(M, kp.e, kp.n);
    cout << "C (ban ma) : " << C << "\n";

    NTL::ZZ M2 = BigInt::Decrypt(C, kp.d, kp.n);
    string recovered = ZZToString(M2);
    cout << "Giai ma    : \"" << recovered << "\"\n";

    cout << "Ket qua    : "
        << (plaintext == recovered ? "THANH CONG [OK]" : "THAT BAI [FAILED]")
        << "\n";
}

// ============================================================
//  INTERACTIVE: Toán học cơ bản
// ============================================================
void interactive_math() {
    printSep("INTERACTIVE: Cac ham toan hoc co ban");

    cout << "[1] GCD\n"
        << "[2] Extended GCD\n"
        << "[3] Modular Inverse\n"
        << "[4] Power Mod\n"
        << "[0] Huy / Quay lai menu\n";
    int choice = readInt("Chon chuc nang (0-4):", 0, 4);
    if (choice == 0) return;

    if (choice == 1) {
        NTL::ZZ a = readZZ("  Nhap a: ");
        NTL::ZZ b = readZZ("  Nhap b: ");
        cout << "  GCD(" << a << ", " << b << ") = " << BigInt::GCD(a, b) << "\n";
    }
    else if (choice == 2) {
        NTL::ZZ a = readZZ("  Nhap a: ");
        NTL::ZZ b = readZZ("  Nhap b: ");
        NTL::ZZ x, y;
        NTL::ZZ g = BigInt::ExtendedGCD(a, b, x, y);
        cout << "  gcd = " << g << ",  x = " << x << ",  y = " << y << "\n";
        cout << "  Kiem tra: " << a << "*" << x << " + " << b << "*" << y
            << " = " << (a * x + b * y) << "\n";
    }
    else if (choice == 3) {
        NTL::ZZ a = readZZ("  Nhap a: ");
        NTL::ZZ m = readZZ("  Nhap m: ");
        try {
            NTL::ZZ inv = BigInt::ModularInverse(a, m);
            cout << "  " << a << "^(-1) mod " << m << " = " << inv << "\n";
            cout << "  Kiem tra: " << a << " * " << inv << " mod " << m
                << " = " << (a * inv) % m << "\n";
        }
        catch (const exception& e) {
            cout << "  [Loi] " << e.what() << "\n";
        }
    }
    else {
        NTL::ZZ base = readZZ("  Nhap base: ");
        NTL::ZZ exp = readZZ("  Nhap exp (>= 0): ");
        NTL::ZZ mod = readZZ("  Nhap mod (> 0): ");
        if (mod <= 0) { cout << "  [Loi] mod phai > 0.\n"; return; }
        cout << "  " << base << "^" << exp << " mod " << mod
            << " = " << BigInt::PowerMod(base, exp, mod) << "\n";
    }
}

// ============================================================
//  INTERACTIVE: Sinh số nguyên tố
// ============================================================
void interactive_prime() {
    printSep("INTERACTIVE: Sinh so nguyen to lon");

    int bits = readInt("Nhap so bit (16 - 2048): ", 16, 2048);
    cout << "Dang sinh so nguyen to " << bits << "-bit...\n";
    NTL::ZZ p = BigInt::GenerateStrongLargePrime(bits);
    cout << "p = " << p << "\n";
    cout << "So bit thuc te: " << NTL::NumBits(p) << "\n";
}

// ============================================================
//  INTERACTIVE: Tạo & kiểm tra cặp khóa RSA
// ============================================================
void interactive_keygen() {
    printSep("INTERACTIVE: Tao bo khoa RSA");

    cout << "[1] Tu dong sinh p, q\n"
        << "[2] Tu nhap p, q\n"
        << "[0] Huy / Quay lai menu\n";
    int choice = readInt("Chon (0-2):", 0, 2);
    if (choice == 0) return;

    NTL::ZZ p, q;
    if (choice == 1) {
        int bits = readInt("Nhap so bit cho moi so nguyen to (16 - 1024): ", 16, 1024);
        cout << "Dang sinh p...\n";
        p = BigInt::GenerateStrongLargePrime(bits);
        cout << "Dang sinh q...\n";
        q = BigInt::GenerateStrongLargePrime(bits);
        while (p == q) {
            cout << "p == q, sinh lai q...\n";
            q = BigInt::GenerateStrongLargePrime(bits);
        }
    }
    else {
        p = readZZ("Nhap p (so nguyen to): ");
        q = readZZ("Nhap q (so nguyen to, p != q): ");
        if (p == q) { cout << "[Loi] p va q phai khac nhau.\n"; return; }
    }

    cout << "\np = " << p << "\n";
    cout << "q = " << q << "\n";

    try {
        BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);
        cout << "\n--- Bo khoa RSA ---\n";
        cout << "n = " << kp.n << "\n";
        cout << "e = " << kp.e << "\n";
        cout << "d = " << kp.d << "\n";
        cout << "So bit cua n: " << NTL::NumBits(kp.n) << "\n";

        NTL::ZZ phi = (p - 1) * (q - 1);
        NTL::ZZ check = (kp.e * kp.d) % phi;
        cout << "Kiem tra: e*d mod phi(n) = " << check
            << (check == 1 ? "  [OK]" : "  [FAILED]") << "\n";
    }
    catch (const exception& e) {
        cout << "[Loi] " << e.what() << "\n";
    }
}

// ============================================================
//  INTERACTIVE: Mã hóa / Giải mã số nguyên
// ============================================================
void interactive_encrypt_number() {
    printSep("INTERACTIVE: Ma hoa / Giai ma so nguyen");

    cout << "--- Nhap khoa RSA ---\n";
    NTL::ZZ n = readZZ("Nhap n (modulus): ");
    NTL::ZZ e = readZZ("Nhap e (public exponent): ");
    NTL::ZZ d = readZZ("Nhap d (private exponent, 0 = bo qua giai ma): ");

    NTL::ZZ M = readZZ("Nhap message M (0 <= M < n): ");
    if (M < 0 || M >= n) {
        cout << "[Loi] M phai thoa 0 <= M < n.\n";
        return;
    }

    NTL::ZZ C = BigInt::Encrypt(M, e, n);
    cout << "Ban ma C = M^e mod n = " << C << "\n";

    if (d > 0) {
        NTL::ZZ M2 = BigInt::Decrypt(C, d, n);
        cout << "Giai ma M' = C^d mod n = " << M2 << "\n";
        cout << "Ket qua: " << (M == M2 ? "THANH CONG [OK]" : "THAT BAI [FAILED]") << "\n";
    }
}

// ============================================================
//  INTERACTIVE: Mã hóa / Giải mã chuỗi văn bản
// ============================================================
void interactive_encrypt_string() {
    printSep("INTERACTIVE: Ma hoa / Giai ma chuoi van ban");

    cout << "--- Nhap khoa RSA ---\n";
    NTL::ZZ n = readZZ("Nhap n (modulus): ");
    NTL::ZZ e = readZZ("Nhap e (public exponent): ");
    NTL::ZZ d = readZZ("Nhap d (private exponent, 0 = bo qua giai ma): ");

    string plaintext = readLine("Nhap chuoi plaintext:");

    NTL::ZZ M = StringToZZ(plaintext);
    cout << "M (so) = " << M << "\n";

    if (M >= n) {
        cout << "[Loi] Message qua lon so voi n. Dung chuoi ngan hon hoac n lon hon.\n";
        return;
    }

    NTL::ZZ C = BigInt::Encrypt(M, e, n);
    cout << "Ban ma C = " << C << "\n";

    if (d > 0) {
        NTL::ZZ M2 = BigInt::Decrypt(C, d, n);
        string recovered = ZZToString(M2);
        cout << "Giai ma    : \"" << recovered << "\"\n";
        cout << "Ket qua    : "
            << (plaintext == recovered ? "THANH CONG [OK]" : "THAT BAI [FAILED]")
            << "\n";
    }
}

// ============================================================
//  INTERACTIVE: Luồng end-to-end (sinh key + mã hóa chuỗi)
// ============================================================
void interactive_full_flow() {
    printSep("INTERACTIVE: Luong RSA day du (sinh khoa + ma hoa chuoi)");

    int bits = readInt("Nhap so bit cho moi so nguyen to (16 - 1024): ", 16, 1024);

    cout << "Dang sinh p...\n";
    NTL::ZZ p = BigInt::GenerateStrongLargePrime(bits);
    cout << "Dang sinh q...\n";
    NTL::ZZ q = BigInt::GenerateStrongLargePrime(bits);
    while (p == q) { cout << "p == q, sinh lai...\n"; q = BigInt::GenerateStrongLargePrime(bits); }

    BigInt::RSAKeyPair kp = BigInt::GenerateKeyPair(p, q);
    cout << "\n[Khoa da tao]\n";
    cout << "n = " << kp.n << "\n";
    cout << "e = " << kp.e << "\n";
    cout << "d = " << kp.d << "\n";

    string plaintext = readLine("Nhap chuoi can ma hoa:");

    NTL::ZZ M = StringToZZ(plaintext);
    if (M >= kp.n) {
        cout << "[Loi] Chuoi qua dai voi kich thuoc khoa hien tai. Tang so bit hoac rut ngan chuoi.\n";
        return;
    }

    NTL::ZZ C = BigInt::Encrypt(M, kp.e, kp.n);
    NTL::ZZ M2 = BigInt::Decrypt(C, kp.d, kp.n);
    string recovered = ZZToString(M2);

    cout << "\nPlaintext  : \"" << plaintext << "\"\n";
    cout << "Ban ma C   : " << C << "\n";
    cout << "Giai ma    : \"" << recovered << "\"\n";
    cout << "Ket qua    : "
        << (plaintext == recovered ? "THANH CONG [OK]" : "THAT BAI [FAILED]")
        << "\n";
}

// ============================================================
//  Menu chính
// ============================================================
void showMainMenu() {
    cout << "\n" << string(60, '-') << "\n";
    cout << "  MENU CHINH\n";
    cout << string(60, '-') << "\n";
    cout << "  [1]  Chay tat ca demo mau (auto)\n";
    cout << "  [2]  Demo toan hoc co ban     (interactive)\n";
    cout << "  [3]  Sinh so nguyen to lon    (interactive)\n";
    cout << "  [4]  Tao bo khoa RSA          (interactive)\n";
    cout << "  [5]  Ma hoa / giai ma so      (interactive)\n";
    cout << "  [6]  Ma hoa / giai ma chuoi   (interactive)\n";
    cout << "  [7]  Luong RSA day du         (interactive)\n";
    cout << "  [0]  Thoat\n";
    cout << string(60, '-') << "\n";
    cout << "  Tip: Gõ 'q' de huy thao tac dang lam  |  'e' de thoat chuong trinh\n";
    cout << string(60, '-') << "\n";
}

// ============================================================
//  main
// ============================================================
int main() {
    cout << "===========================================\n";
    cout << "  BTL3 - He ma RSA  |  NTL + C++\n";
    cout << "===========================================\n";

    try {
        while (true) {
            showMainMenu();
            int choice = readInt("Chon chuc nang: ", 0, 7);

            try {
                switch (choice) {
                case 0:
                    printSep("HOAN THANH");
                    return 0;

                case 1:
                    demo_math();
                    demo_prime();
                    demo_keygen(512);
                    demo_encrypt_number();
                    demo_encrypt_string();
                    printSep("HOAN THANH DEMO MAU");
                    break;

                case 2: interactive_math();             break;
                case 3: interactive_prime();            break;
                case 4: interactive_keygen();           break;
                case 5: interactive_encrypt_number();   break;
                case 6: interactive_encrypt_string();   break;
                case 7: interactive_full_flow();        break;
                }
            }
            catch (const CancelException&) {
                cout << "\n  [Huy] Da quay ve menu chinh.\n";
            }
            // ExitException KHÔNG bị bắt ở đây → nổi lên catch ngoài
        }
    }
    catch (const ExitException&) {
        printSep("THOAT CHUONG TRINH");
    }
    return 0;
}
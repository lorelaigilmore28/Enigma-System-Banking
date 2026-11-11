// ============================================================================
//  MINI BANKING SYSTEM  (C++)
//  Change BANK_NAME and the MEMBERS list below to your own group details.
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <random>
#include <ctime>
#include <cctype>
#include <cstdlib>

using namespace std;

// ---------------------------- GROUP DETAILS --------------------------------
const string BANK_NAME = "SAMPLE BANK";            // <-- your Group Name / Bank Name
const vector<string> MEMBERS = {                   // <-- your group members
    "Member 1",
    "Member 2",
    "Member 3",
    "Member 4"
};

// ---------------------------- CONSTANTS ------------------------------------
const double SA_INITIAL_MIN = 5000.0;
const double CA_INITIAL_MIN = 10000.0;
const double SA_TXN_MIN = 300.0;
const double CA_TXN_MIN = 500.0;

// ---------------------------- DATA -----------------------------------------
struct Account {
    bool exists = false;
    string accountNumber;
    string firstName, lastName, middleName;
    string address;
    int bMonth = 0, bDay = 0, bYear = 0;
    char gender = 'M';
    bool isSavings = true;
    double initialDeposit = 0.0;
    double balance = 0.0;
    string pin;

    string typeName() const { return isSavings ? "Savings Account" : "Current Account"; }
    double minTxn() const { return isSavings ? SA_TXN_MIN : CA_TXN_MIN; }
    string fullName() const { return firstName + " " + middleName + " " + lastName; }
};

Account account;   // the system holds the current client's account

// ---------------------------- UI HELPERS -----------------------------------
void printBanner(const string& screenTitle) {
    cout << "\n==============================================\n";
    cout << "   " << BANK_NAME << "\n";
    cout << "==============================================\n";
    cout << "   " << screenTitle << "\n";
    cout << "----------------------------------------------\n";
}

string readLine(const string& prompt) {
    string s;
    cout << prompt;
    if (!getline(cin, s)) {           // input closed -> exit gracefully
        cout << "\nInput ended. Exiting program.\n";
        exit(0);
    }
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

void pause() { readLine("\nPress Enter to return to the menu..."); }

bool parseAmount(const string& s, double& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    out = strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0';
}

bool parseInt(const string& s, int& out) {
    if (s.empty() || s.size() > 9) return false;
    for (char c : s) if (!isdigit((unsigned char)c)) return false;
    out = atoi(s.c_str());
    return true;
}

bool askYesNo(const string& prompt) {
    while (true) {
        string a = readLine(prompt + " (Y/N): ");
        if (a == "Y" || a == "y" || a == "yes" || a == "Yes" || a == "YES") return true;
        if (a == "N" || a == "n" || a == "no" || a == "No" || a == "NO") return false;
        cout << "Please answer Y or N.\n";
    }
}

string money(double v) {
    ostringstream o;
    o << fixed << setprecision(2) << v;
    string s = o.str();
    // add thousands separators
    size_t dot = s.find('.');
    int i = (int)dot - 3;
    while (i > 0) { s.insert(i, ","); i -= 3; }
    return "PHP " + s;
}

// ---------------------------- VALIDATION -----------------------------------
bool validName(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isalpha((unsigned char)c) && c != ' ' && c != '-' && c != '.' && c != '\'') return false;
    return true;
}

bool isLeap(int y) { return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); }

int daysInMonth(int m, int y) {
    static const int d[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (m == 2 && isLeap(y)) return 29;
    return d[m - 1];
}

void today(int& d, int& m, int& y) {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    d = lt->tm_mday; m = lt->tm_mon + 1; y = lt->tm_year + 1900;
}

int ageOf(int bd, int bm, int by) {
    int d, m, y;
    today(d, m, y);
    int age = y - by;
    if (m < bm || (m == bm && d < bd)) age--;
    return age;
}

// Reads a birthday (MM/DD/YYYY). Returns false if the person is under 18.
bool readBirthday(int& bm, int& bd, int& by) {
    int td, tm_, ty;
    today(td, tm_, ty);
    while (true) {
        string s = readLine("Birthday (MM/DD/YYYY): ");
        int m, d, y;
        size_t p1 = s.find('/');
        size_t p2 = (p1 == string::npos) ? string::npos : s.find('/', p1 + 1);
        if (p1 == string::npos || p2 == string::npos ||
            !parseInt(s.substr(0, p1), m) ||
            !parseInt(s.substr(p1 + 1, p2 - p1 - 1), d) ||
            !parseInt(s.substr(p2 + 1), y)) {
            cout << "Invalid format. Please use MM/DD/YYYY (e.g. 05/20/2000).\n";
            continue;
        }
        if (y < 1900 || y > ty || m < 1 || m > 12 || d < 1 || d > daysInMonth(m, y)) {
            cout << "That is not a valid date. Please try again.\n";
            continue;
        }
        if (ageOf(d, m, y) < 0) {
            cout << "Birthday cannot be in the future. Please try again.\n";
            continue;
        }
        bm = m; bd = d; by = y;
        return ageOf(d, m, y) >= 18;
    }
}

string birthdayStr(const Account& a) {
    static const char* months[] = {"January","February","March","April","May","June",
                                   "July","August","September","October","November","December"};
    return string(months[a.bMonth - 1]) + " " + to_string(a.bDay) + ", " + to_string(a.bYear);
}

string generateAccountNumber() {
    static mt19937 rng((unsigned)time(nullptr) ^ (unsigned)clock());
    uniform_int_distribution<long long> dist(1000000000LL, 9999999999LL);
    return to_string(dist(rng));
}

// ---------------------------- PIN CHECK ------------------------------------
// Returns true when the correct PIN was entered, false if the user backed out.
bool verifyAccess() {
    if (!account.exists) {
        cout << "\nNo account found. Please open a new account first (option 1).\n";
        return false;
    }
    while (true) {
        string pin = readLine("Enter your 6-digit PIN (or 0 to go back): ");
        if (pin == "0") return false;
        if (pin == account.pin) return true;
        cout << "Incorrect PIN. Please try again.\n";
    }
}

// ---------------------------- MENU ACTIONS ---------------------------------
void openAccount() {
    printBanner("OPEN A NEW ACCOUNT");
    if (account.exists) {
        cout << "An account is already open in this system.\n"
                "Please close the existing account (option 6) before opening a new one.\n";
        return;
    }

    Account a;
    while (true) { a.firstName  = readLine("First name  : "); if (validName(a.firstName))  break; cout << "Invalid name. Use letters only.\n"; }
    while (true) { a.middleName = readLine("Middle name : "); if (validName(a.middleName)) break; cout << "Invalid name. Use letters only.\n"; }
    while (true) { a.lastName   = readLine("Last name   : "); if (validName(a.lastName))   break; cout << "Invalid name. Use letters only.\n"; }

    while (true) { a.address = readLine("Address     : "); if (!a.address.empty()) break; cout << "Address cannot be empty.\n"; }

    if (!readBirthday(a.bMonth, a.bDay, a.bYear)) {
        cout << "\nSorry, your age is not yet allowed to open an account.\n"
                "You must be at least 18 years old.\n";
        return;
    }

    while (true) {
        string g = readLine("Gender (M/F): ");
        if (g == "M" || g == "m") { a.gender = 'M'; break; }
        if (g == "F" || g == "f") { a.gender = 'F'; break; }
        cout << "Invalid input. Only M or F is allowed.\n";
    }

    cout << "\nAccount types:\n"
         << "  1. Savings Account  (initial deposit of " << money(SA_INITIAL_MIN) << " or more)\n"
         << "  2. Current Account  (initial deposit of " << money(CA_INITIAL_MIN) << " or more)\n";
    while (true) {
        string t = readLine("Select account type (1/2): ");
        if (t == "1") { a.isSavings = true;  break; }
        if (t == "2") { a.isSavings = false; break; }
        cout << "Invalid choice. Enter 1 or 2.\n";
    }

    double need = a.isSavings ? SA_INITIAL_MIN : CA_INITIAL_MIN;
    while (true) {
        string s = readLine("Initial deposit: ");
        double amt;
        if (!parseAmount(s, amt)) { cout << "Please enter a valid number.\n"; continue; }
        if (amt < need) {
            cout << "The initial deposit is not sufficient. The required initial deposit for a "
                 << a.typeName() << " is " << money(need) << " or more.\n";
            continue;
        }
        a.initialDeposit = amt;
        a.balance = amt;
        break;
    }

    while (true) {
        string p = readLine("Set a 6-digit PIN: ");
        bool ok = p.size() == 6;
        for (char c : p) if (!isdigit((unsigned char)c)) ok = false;
        if (!ok) { cout << "PIN must be exactly 6 digits.\n"; continue; }
        string p2 = readLine("Confirm PIN     : ");
        if (p != p2) { cout << "PINs do not match. Please try again.\n"; continue; }
        a.pin = p;
        break;
    }

    a.accountNumber = generateAccountNumber();
    a.exists = true;
    account = a;

    cout << "\nAccount created successfully!\n"
         << "Account Name   : " << account.fullName() << "\n"
         << "Account Number : " << account.accountNumber << "\n"
         << "Account Type   : " << account.typeName() << "\n"
         << "Balance        : " << money(account.balance) << "\n"
         << "Please keep your account number and PIN safe.\n";
}

void balanceInquiry() {
    printBanner("BALANCE INQUIRY");
    if (!verifyAccess()) return;
    cout << "\nAccount Number  : " << account.accountNumber << "\n"
         << "Current Balance : " << money(account.balance) << "\n";
}

void deposit() {
    printBanner("DEPOSIT");
    if (!verifyAccess()) return;
    double minAmt = account.minTxn();
    cout << "\nMinimum deposit for a " << account.typeName() << ": " << money(minAmt) << "\n";
    while (true) {
        string s = readLine("Amount to deposit (or 0 to cancel): ");
        double amt;
        if (!parseAmount(s, amt)) { cout << "The entered amount is not valid. Please enter a number or 0 to cancel.\n"; continue; }
        if (amt == 0) { cout << "Transaction cancelled.\n"; return; }
        if (amt < 0 || amt < minAmt) {
            cout << "The entered amount is not valid. Please enter a valid amount (minimum "
                 << money(minAmt) << ") or 0 to cancel the transaction.\n";
            continue;
        }
        if (!askYesNo("Confirm deposit of " + money(amt) + "?")) {
            cout << "Transaction cancelled.\n";
            return;
        }
        account.balance += amt;
        cout << "\nTransaction successful! You deposited " << money(amt) << ".\n"
             << "New balance: " << money(account.balance) << "\n";
        return;
    }
}

void withdraw() {
    printBanner("WITHDRAW");
    if (!verifyAccess()) return;
    double minAmt = account.minTxn();
    cout << "\nCurrent balance: " << money(account.balance) << "\n"
         << "Minimum withdrawal for a " << account.typeName() << ": " << money(minAmt) << "\n";
    while (true) {
        string s = readLine("Amount to withdraw (or 0 to cancel): ");
        double amt;
        if (!parseAmount(s, amt)) { cout << "The entered amount is not valid. Please enter a number or 0 to cancel.\n"; continue; }
        if (amt == 0) { cout << "Transaction cancelled.\n"; return; }
        if (amt < 0 || amt < minAmt || amt > account.balance) {
            cout << "The entered amount is not valid. Please enter a valid amount (minimum "
                 << money(minAmt) << ", not more than your balance) or 0 to cancel the transaction.\n";
            continue;
        }
        if (!askYesNo("Confirm withdrawal of " + money(amt) + "?")) {
            cout << "Transaction cancelled.\n";
            return;
        }
        account.balance -= amt;
        cout << "\nTransaction successful! You withdrew " << money(amt) << ".\n"
             << "Remaining balance: " << money(account.balance) << "\n";
        return;
    }
}

void viewAccount() {
    printBanner("VIEW ACCOUNT INFORMATION");
    if (!verifyAccess()) return;
    cout << "\nAccount Number  : " << account.accountNumber << "\n"
         << "Full Name       : " << account.fullName() << "\n"
         << "Address         : " << account.address << "\n"
         << "Birthday        : " << birthdayStr(account) << "\n"
         << "Gender          : " << (account.gender == 'M' ? "Male" : "Female") << "\n"
         << "Account Type    : " << account.typeName() << "\n"
         << "Initial Deposit : " << money(account.initialDeposit) << "\n"
         << "Current Balance : " << money(account.balance) << "\n";
}

void closeAccount() {
    printBanner("CLOSE ACCOUNT");
    if (!verifyAccess()) return;
    cout << "\nClosing your account will return your remaining balance of "
         << money(account.balance) << " to you and erase all account information.\n";
    if (!askYesNo("Are you sure you want to close your account?")) {
        cout << "Account closure cancelled.\n";
        return;
    }
    cout << "\nPlease collect your money: " << money(account.balance) << "\n";
    account = Account();   // clear out all information
    cout << "Your account is now closed. Thank you for banking with " << BANK_NAME << ".\n";
}

void exitProgram() {
    printBanner("THANK YOU");
    cout << "Thank you for using our banking system!\n\n"
         << "Group / Bank Name: " << BANK_NAME << "\n"
         << "Members:\n";
    for (const string& m : MEMBERS) cout << "  - " << m << "\n";
    cout << "==============================================\n";
}

// ---------------------------- MAIN -----------------------------------------
int main() {
    while (true) {
        printBanner("MAIN MENU");
        cout << "  1. Open a New Account\n"
             << "  2. Balance Inquiry\n"
             << "  3. Deposit\n"
             << "  4. Withdraw\n"
             << "  5. View Account Information\n"
             << "  6. Close Account\n"
             << "  7. Exit Program\n"
             << "----------------------------------------------\n";
        string choice = readLine("Select an option (1-7): ");

        if      (choice == "1") openAccount();
        else if (choice == "2") balanceInquiry();
        else if (choice == "3") deposit();
        else if (choice == "4") withdraw();
        else if (choice == "5") viewAccount();
        else if (choice == "6") closeAccount();
        else if (choice == "7") { exitProgram(); return 0; }
        else { cout << "Invalid option. Please enter a number from 1 to 7.\n"; continue; }

        pause();   // then the menu is redisplayed
    }
}
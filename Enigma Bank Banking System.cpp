#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

const string BANK_NAME = "Enigma Bank";

const double SA_INITIAL_MIN = 5000.0;
const double CA_INITIAL_MIN = 10000.0;
const double SA_TXN_MIN = 300.0;
const double CA_TXN_MIN = 500.0;

const int TODAY_MONTH = 11;
const int TODAY_DAY = 11;
const int TODAY_YEAR = 2026;

bool hasAccount = false;
long long accountNumber = 0;
string firstName, middleName, lastName, address;
int birthMonth = 0, birthDay = 0, birthYear = 0;
char gender = 'M';
int accountType = 1;
double initialDeposit = 0.0;
double balance = 0.0;
int pin = 0;

void printLine(char symbol) {
    for (int i = 0; i < 46; i++) {
        cout << symbol;
    }
    cout << endl;
}

void printHeader(string screenTitle) {
    cout << endl;
    printLine('=');
    cout << "   " << BANK_NAME << endl;
    printLine('=');
    cout << "   " << screenTitle << endl;
    printLine('-');
}

string accountTypeName() {
    if (accountType == 1) {
        return "Savings Account";
    } else {
        return "Current Account";
    }
}

double minimumTransaction() {
    if (accountType == 1) {
        return SA_TXN_MIN;
    } else {
        return CA_TXN_MIN;
    }
}

string monthName(int month) {
    switch (month) {
        case 1:  return "January";
        case 2:  return "February";
        case 3:  return "March";
        case 4:  return "April";
        case 5:  return "May";
        case 6:  return "June";
        case 7:  return "July";
        case 8:  return "August";
        case 9:  return "September";
        case 10: return "October";
        case 11: return "November";
        default: return "December";
    }
}

void checkEndOfInput() {
    if (cin.eof()) {
        cout << endl << "Input ended. Exiting program." << endl;
        exit(0);
    }
}

int readInt(string prompt) {
    int value;
    cout << prompt;
    cin >> value;
    while (cin.fail()) {
        checkEndOfInput();
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Please enter a whole number: ";
        cin >> value;
    }
    cin.ignore(1000, '\n');
    return value;
}

double readDouble(string prompt) {
    double value;
    cout << prompt;
    cin >> value;
    while (cin.fail()) {
        checkEndOfInput();
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Please enter a number: ";
        cin >> value;
    }
    cin.ignore(1000, '\n');
    return value;
}

string readText(string prompt) {
    string text;
    do {
        cout << prompt;
        getline(cin, text);
        checkEndOfInput();
        if (text == "") {
            cout << "This field cannot be empty." << endl;
        }
    } while (text == "");
    return text;
}

bool confirm(string prompt) {
    char answer;
    do {
        cout << prompt << " (Y/N): ";
        cin >> answer;
        checkEndOfInput();
        cin.ignore(1000, '\n');
        if (answer != 'Y' && answer != 'y' && answer != 'N' && answer != 'n') {
            cout << "Please answer Y or N." << endl;
        }
    } while (answer != 'Y' && answer != 'y' && answer != 'N' && answer != 'n');

    return (answer == 'Y' || answer == 'y');
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int month, int year) {
    switch (month) {
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            if (isLeapYear(year)) {
                return 29;
            } else {
                return 28;
            }
        default:
            return 31;
    }
}

int computeAge(int month, int day, int year) {
    int age = TODAY_YEAR - year;
    if (TODAY_MONTH < month || (TODAY_MONTH == month && TODAY_DAY < day)) {
        age--;
    }
    return age;
}

int countDigits(long long number) {
    if (number < 10) {
        return 1;
    }
    return 1 + countDigits(number / 10);
}

double roundToCents(double amount) {
    return floor(amount * 100.0 + 0.5) / 100.0;
}

long long generateAccountNumber() {
    long long number = 1 + rand() % 9;
    for (int i = 1; i < 10; i++) {
        number = number * 10 + rand() % 10;
    }
    return number;
}

bool askPin() {
    int entered = readInt("Enter your 6-digit PIN (or 0 to go back): ");
    if (entered == 0) {
        return false;
    }
    if (entered == pin) {
        return true;
    }
    cout << "Incorrect PIN. Please try again." << endl;
    return askPin();
}

bool checkAccess() {
    if (!hasAccount) {
        cout << endl << "No account found. Please open a new account first (option 1)." << endl;
        return false;
    }
    return askPin();
}

void openAccount() {
    printHeader("OPEN A NEW ACCOUNT");

    if (hasAccount) {
        cout << "An account is already open in this system." << endl;
        cout << "Please close it first (option 6) before opening a new one." << endl;
        return;
    }

    string first = readText("First name  : ");
    string middle = readText("Middle name : ");
    string last = readText("Last name   : ");
    string addr = readText("Address     : ");

    int month, day, year;
    bool validDate;
    do {
        cout << "Birthday" << endl;
        month = readInt("  Month (1-12) : ");
        day = readInt("  Day          : ");
        year = readInt("  Year (YYYY)  : ");

        validDate = true;
        if (month < 1 || month > 12) {
            validDate = false;
        } else if (year < 1900 || year > TODAY_YEAR) {
            validDate = false;
        } else if (day < 1 || day > daysInMonth(month, year)) {
            validDate = false;
        } else if (computeAge(month, day, year) < 0) {
            validDate = false;
        }

        if (!validDate) {
            cout << "That is not a valid date. Please try again." << endl;
        }
    } while (!validDate);

    if (computeAge(month, day, year) < 18) {
        cout << endl << "Sorry, your age is not yet allowed to open an account." << endl;
        cout << "You must be at least 18 years old." << endl;
        return;
    }

    char g;
    do {
        cout << "Gender (M/F): ";
        cin >> g;
        checkEndOfInput();
        cin.ignore(1000, '\n');
        if (g != 'M' && g != 'm' && g != 'F' && g != 'f') {
            cout << "Invalid input. Only M or F is allowed." << endl;
        }
    } while (g != 'M' && g != 'm' && g != 'F' && g != 'f');

    if (g == 'm') {
        g = 'M';
    } else if (g == 'f') {
        g = 'F';
    }

    cout << endl << "Account types:" << endl;
    cout << "  1. Savings Account (initial deposit of " << SA_INITIAL_MIN << " or more)" << endl;
    cout << "  2. Current Account (initial deposit of " << CA_INITIAL_MIN << " or more)" << endl;
    int type;
    do {
        type = readInt("Select account type (1 or 2): ");
        if (type != 1 && type != 2) {
            cout << "Invalid choice. Please enter 1 or 2." << endl;
        }
    } while (type != 1 && type != 2);

    double required;
    switch (type) {
        case 1:
            required = SA_INITIAL_MIN;
            break;
        default:
            required = CA_INITIAL_MIN;
            break;
    }

    double deposit;
    do {
        deposit = readDouble("Initial deposit: ");
        if (deposit < required) {
            cout << "The initial deposit is not sufficient. A ";
            if (type == 1) {
                cout << "Savings Account";
            } else {
                cout << "Current Account";
            }
            cout << " needs " << required << " or more." << endl;
        }
    } while (deposit < required);
    deposit = roundToCents(deposit);

    int newPin;
    do {
        newPin = readInt("Set a 6-digit PIN: ");
        if (newPin <= 0 || countDigits(newPin) != 6) {
            cout << "PIN must be a 6-digit number." << endl;
        }
    } while (newPin <= 0 || countDigits(newPin) != 6);

    firstName = first;
    middleName = middle;
    lastName = last;
    address = addr;
    birthMonth = month;
    birthDay = day;
    birthYear = year;
    gender = g;
    accountType = type;
    initialDeposit = deposit;
    balance = deposit;
    pin = newPin;
    accountNumber = generateAccountNumber();
    hasAccount = true;

    cout << fixed << setprecision(2);
    cout << endl << "Account created successfully!" << endl;
    cout << "Account Name   : " << firstName << " " << middleName << " " << lastName << endl;
    cout << "Account Number : " << accountNumber << endl;
    cout << "Account Type   : " << accountTypeName() << endl;
    cout << "Balance        : PHP " << balance << endl;
    cout << "Please keep your account number and PIN safe." << endl;
}

void balanceInquiry() {
    printHeader("BALANCE INQUIRY");
    if (!checkAccess()) {
        return;
    }
    cout << fixed << setprecision(2);
    cout << endl << "Account Number  : " << accountNumber << endl;
    cout << "Current Balance : PHP " << balance << endl;
}

void deposit() {
    printHeader("DEPOSIT");
    if (!checkAccess()) {
        return;
    }

    double minimum = minimumTransaction();
    cout << fixed << setprecision(2);
    cout << endl << "Minimum deposit for a " << accountTypeName() << ": PHP " << minimum << endl;

    double amount;
    bool valid;
    do {
        amount = readDouble("Amount to deposit (or 0 to cancel): ");
        if (amount == 0) {
            cout << "Transaction cancelled." << endl;
            return;
        }
        valid = (amount >= minimum);
        if (!valid) {
            cout << "The entered amount is not valid. Please enter a valid amount (minimum PHP "
                 << minimum << ") or enter 0 to cancel." << endl;
        }
    } while (!valid);

    amount = roundToCents(amount);
    cout << "You are about to deposit PHP " << amount << "." << endl;
    if (confirm("Proceed with the deposit?")) {
        balance = balance + amount;
        cout << endl << "Transaction successful! You deposited PHP " << amount << "." << endl;
        cout << "New balance: PHP " << balance << endl;
    } else {
        cout << "Transaction cancelled." << endl;
    }
}

void withdraw() {
    printHeader("WITHDRAW");
    if (!checkAccess()) {
        return;
    }

    double minimum = minimumTransaction();
    cout << fixed << setprecision(2);
    cout << endl << "Current balance: PHP " << balance << endl;
    cout << "Minimum withdrawal for a " << accountTypeName() << ": PHP " << minimum << endl;

    double amount;
    bool valid;
    do {
        amount = readDouble("Amount to withdraw (or 0 to cancel): ");
        if (amount == 0) {
            cout << "Transaction cancelled." << endl;
            return;
        }
        valid = (amount >= minimum && amount <= balance);
        if (!valid) {
            cout << "The entered amount is not valid. Please enter a valid amount (minimum PHP "
                 << minimum << ", not more than your balance) or enter 0 to cancel." << endl;
        }
    } while (!valid);

    amount = roundToCents(amount);
    cout << "You are about to withdraw PHP " << amount << "." << endl;
    if (confirm("Proceed with the withdrawal?")) {
        balance = balance - amount;
        cout << endl << "Transaction successful! You withdrew PHP " << amount << "." << endl;
        cout << "Remaining balance: PHP " << balance << endl;
    } else {
        cout << "Transaction cancelled." << endl;
    }
}

void viewAccountInformation() {
    printHeader("VIEW ACCOUNT INFORMATION");
    if (!checkAccess()) {
        return;
    }
    cout << fixed << setprecision(2);
    cout << endl << "Account Number  : " << accountNumber << endl;
    cout << "Full Name       : " << firstName << " " << middleName << " " << lastName << endl;
    cout << "Address         : " << address << endl;
    cout << "Birthday        : " << monthName(birthMonth) << " " << birthDay << ", " << birthYear << endl;
    cout << "Gender          : ";
    if (gender == 'M') {
        cout << "Male" << endl;
    } else {
        cout << "Female" << endl;
    }
    cout << "Account Type    : " << accountTypeName() << endl;
    cout << "Initial Deposit : PHP " << initialDeposit << endl;
    cout << "Current Balance : PHP " << balance << endl;
}

void closeAccount() {
    printHeader("CLOSE ACCOUNT");
    if (!checkAccess()) {
        return;
    }

    cout << fixed << setprecision(2);
    cout << endl << "Closing your account will return your balance of PHP " << balance
         << " and erase all account information." << endl;

    if (confirm("Are you sure you want to close your account?")) {
        cout << endl << "Please collect your money: PHP " << balance << endl;

        hasAccount = false;
        accountNumber = 0;
        firstName = "";
        middleName = "";
        lastName = "";
        address = "";
        birthMonth = 0;
        birthDay = 0;
        birthYear = 0;
        gender = 'M';
        accountType = 1;
        initialDeposit = 0.0;
        balance = 0.0;
        pin = 0;

        cout << "Your account is now closed. Thank you for banking with " << BANK_NAME << "." << endl;
    } else {
        cout << "Account closure cancelled." << endl;
    }
}

void exitProgram() {
    printHeader("THANK YOU");
    cout << "Thank you for using our banking system!" << endl << endl;
    cout << "Group / Bank Name: " << BANK_NAME << endl;
    cout << "Members67:" << endl;
    cout << "  - Iverson Fontanilla" << endl;
    cout << "  - Kian Hizon" << endl;
    cout << "  - Pablo Tirona" << endl;
    cout << "  - Manuel Roska" << endl;
    printLine('=');
}

void showMenu() {
    printHeader("MAIN MENU");
    cout << "  1. Open a New Account" << endl;
    cout << "  2. Balance Inquiry" << endl;
    cout << "  3. Deposit" << endl;
    cout << "  4. Withdraw" << endl;
    cout << "  5. View Account Information" << endl;
    cout << "  6. Close Account" << endl;
    cout << "  7. Exit Program" << endl;
    printLine('-');
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    int choice;

    do {
        showMenu();
        choice = readInt("Select an option (1-7): ");

        switch (choice) {
            case 1:
                openAccount();
                break;
            case 2:
                balanceInquiry();
                break;
            case 3:
                deposit();
                break;
            case 4:
                withdraw();
                break;
            case 5:
                viewAccountInformation();
                break;
            case 6:
                closeAccount();
                break;
            case 7:
                exitProgram();
                break;
            default:
                cout << "Invalid option. Please enter a number from 1 to 7." << endl;
        }
    } while (choice != 7);

    return 0;
}

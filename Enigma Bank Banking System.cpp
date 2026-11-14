// Include the libraries for input, output, text, money display, math, and dates.
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Allow standard library names such as cout and string without the std prefix.
using namespace std;

// Store the bank name as a constant so it stays the same throughout the program.
const string BANK_NAME = "ENIGMA BANKING SYSTEM";

// Set the opening deposit and transaction minimums for both account types.
const double SA_INITIAL_MIN = 5000.0;
const double CA_INITIAL_MIN = 10000.0;
const double SA_TXN_MIN = 300.0;
const double CA_TXN_MIN = 500.0;

// Store one account in memory so the banking functions can share its information.
bool hasAccount = false;
long long accountNumber = 0;
string firstName, middleName, lastName, address;
int birthMonth = 0, birthDay = 0, birthYear = 0;
char gender = 'M';
int accountType = 1;
double initialDeposit = 0.0;
double balance = 0.0;
int pin = 0;

// Print a separator made from 46 copies of the chosen character.
void printLine(char symbol) {
    cout << string(46, symbol) << endl;
}

// Display a consistent bank heading and title for each screen.
void printHeader(string screenTitle) {
    cout << endl;
    printLine('=');
    cout << "   " << BANK_NAME << endl;
    printLine('=');
    cout << "   " << screenTitle << endl;
    printLine('-');
}

// Return the readable name of the active account type.
string accountTypeName() {
    if (accountType == 1) {
        return "Savings Account";
    }
    return "Current Account";
}

// Choose the minimum deposit or withdrawal amount for the active account.
double minimumTransaction() {
    if (accountType == 1) {
        return SA_TXN_MIN;
    }
    return CA_TXN_MIN;
}

// Convert a validated month number into its name for the account display.
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

// Exit cleanly if the input stream has ended instead of retrying forever.
void checkEndOfInput() {
    if (cin.eof()) {
        cout << endl << "Input ended. Exiting program." << endl;
        exit(0);
    }
}

// Read a whole number and retry when numeric input fails.
int readInt(string prompt) {
    int value;
    cout << prompt;
    // Repeat until extraction of a whole number succeeds.
    while (!(cin >> value)) {
        checkEndOfInput();
        // Reset the input error and discard the invalid line before another attempt.
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Please enter a whole number: ";
    }
    // Remove the leftover newline before a later text input operation.
    cin.ignore(1000, '\n');
    return value;
}

// Read a decimal amount and retry when numeric input fails.
double readDouble(string prompt) {
    double value;
    cout << prompt;
    // Repeat until extraction of a decimal number succeeds.
    while (!(cin >> value)) {
        checkEndOfInput();
        // Clear the error and discard the invalid line so the user can try again.
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    // Discard the remaining line, including its newline.
    cin.ignore(1000, '\n');
    return value;
}

// Read a full line of text, including spaces, and reject an empty line.
string readText(string prompt) {
    string text;
    // Keep asking until a nonempty line is returned.
    while (true) {
        cout << prompt;
        getline(cin, text);
        checkEndOfInput();
        // Accept the text once it contains at least one character.
        if (!text.empty()) {
            return text;
        }
        cout << "This field cannot be empty." << endl;
    }
}

// Ask for confirmation and return true for Y or false for N.
bool confirm(string prompt) {
    char answer;
    // Repeat the question until the answer is Y or N, ignoring letter case.
    while (true) {
        cout << prompt << " (Y/N): ";
        cin >> answer;
        checkEndOfInput();
        cin.ignore(1000, '\n');
        // Approve the action when the user answers yes.
        if (answer == 'Y' || answer == 'y') {
            return true;
        }
        // Cancel the action when the user answers no.
        if (answer == 'N' || answer == 'n') {
            return false;
        }
        cout << "Please answer Y or N." << endl;
    }
}

// Apply the leap-year rules, including the century and 400-year exceptions.
bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Return the number of days in a validated month, accounting for leap years.
int daysInMonth(int month, int year) {
    // Group the 30-day months, handle February, and use 31 for the other months.
    switch (month) {
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            // February has 29 days in a leap year and 28 otherwise.
            if (isLeapYear(year)) {
                return 29;
            }
            return 28;
        default:
            return 31;
    }
}

// Read the current local calendar date from the computer instead of a fixed date.
tm getCurrentDate() {
    time_t now = time(0);
    // Convert the current time into calendar fields using the address of now.
    tm* today = localtime(&now);
    // Stop with an error message if the date conversion fails.
    if (today == nullptr) {
        cerr << "Unable to read the current date." << endl;
        exit(EXIT_FAILURE);
    }
    // Return a copy of the calendar information pointed to by today.
    return *today;
}

// Calculate completed years of age from the birthday and the current date.
int computeAge(int month, int day, int year) {
    tm today = getCurrentDate();
    // Convert tm fields: months start at zero and years are counted from 1900.
    int currentMonth = today.tm_mon + 1;
    int age = today.tm_year + 1900 - year;
    // Subtract one year if this year's birthday has not happened yet.
    if (currentMonth < month || (currentMonth == month && today.tm_mday < day)) {
        age--;
    }
    return age;
}

// Check month, year, day limits, and whether the birthday is in the future.
bool isValidBirthday(int month, int day, int year) {
    // All conditions must pass; earlier checks protect later date calculations.
    return month >= 1 && month <= 12
        && year >= 1900 && year <= getCurrentDate().tm_year + 1900
        && day >= 1 && day <= daysInMonth(month, year)
        && computeAge(month, day, year) >= 0;
}

// Count digits recursively to help validate a positive six-digit PIN.
int countDigits(long long number) {
    // Base case: a number below ten is counted as one digit.
    if (number < 10) {
        return 1;
    }
    // Count the final digit, remove it by dividing by ten, and count the rest.
    return 1 + countDigits(number / 10);
}

// Round positive monetary amounts to two decimal places.
double roundToCents(double amount) {
    return floor(amount * 100.0 + 0.5) / 100.0;
}

// Generate a ten-digit random account number; duplicate numbers are not checked.
long long generateAccountNumber() {
    // Choose a first digit from 1 to 9 so the account number cannot start with zero.
    long long number = 1 + rand() % 9;
    // Append nine more random digits, each between 0 and 9.
    for (int i = 1; i < 10; i++) {
        number = number * 10 + rand() % 10;
    }
    return number;
}

// Check the PIN repeatedly and allow zero to return to the menu.
bool askPin() {
    // Keep requesting a PIN until the user succeeds or chooses to go back.
    while (true) {
        int entered = readInt("Enter your 6-digit PIN (or 0 to go back): ");
        // Treat zero as a request to cancel access.
        if (entered == 0) {
            return false;
        }
        // Allow access when the entered PIN matches the stored PIN.
        if (entered == pin) {
            return true;
        }
        cout << "Incorrect PIN. Please try again." << endl;
    }
}

// Require an existing account and a correct PIN before a protected action.
bool checkAccess() {
    // Reject access when no account is currently open.
    if (!hasAccount) {
        cout << endl << "No account found. Please open a new account first (option 1)." << endl;
        return false;
    }
    // Pass the PIN-check result back to the requesting banking function.
    return askPin();
}

// Collect and validate customer details before creating the single active account.
void openAccount() {
    printHeader("OPEN A NEW ACCOUNT");

    // Allow only one account to be opened during each program session.
    if (hasAccount) {
        cout << "An account is already open in this system." << endl;
        cout << "Only one account can be opened per session." << endl;
        return;
    }

    // Keep form entries in local variables until account creation succeeds.
    string first = readText("First name  : ");
    string middle = readText("Middle name : ");
    string last = readText("Last name   : ");
    string addr = readText("Address     : ");

    // Repeat birthday entry until the date exists and is not in the future.
    int month, day, year;
    bool validDate;
    do {
        cout << "Birthday" << endl;
        month = readInt("  Month (1-12) : ");
        day = readInt("  Day          : ");
        year = readInt("  Year (YYYY)  : ");

        validDate = isValidBirthday(month, day, year);

        // Explain the invalid date before repeating the birthday form.
        if (!validDate) {
            cout << "That is not a valid date. Please try again." << endl;
        }
    } while (!validDate);

    // End account creation if the customer is younger than eighteen.
    if (computeAge(month, day, year) < 18) {
        cout << endl << "Sorry, your age is not yet allowed to open an account." << endl;
        cout << "You must be at least 18 years old." << endl;
        return;
    }

    // Accept only M or F for gender, including their lowercase forms.
    char g;
    do {
        cout << "Gender (M/F): ";
        cin >> g;
        checkEndOfInput();
        cin.ignore(1000, '\n');

        // Normalize lowercase input so the saved character is always uppercase.
        if (g == 'm') {
            g = 'M';
        } else if (g == 'f') {
            g = 'F';
        }
        // Report unsupported gender input and repeat the prompt.
        if (g != 'M' && g != 'F') {
            cout << "Invalid input. Only M or F is allowed." << endl;
        }
    } while (g != 'M' && g != 'F');

    // Display the available account types and their opening deposit requirements.
    cout << endl << "Account types:" << endl;
    cout << "  1. Savings Account (initial deposit of " << SA_INITIAL_MIN << " or more)" << endl;
    cout << "  2. Current Account (initial deposit of " << CA_INITIAL_MIN << " or more)" << endl;
    int type;
    // Accept only option 1 for savings or option 2 for current.
    do {
        type = readInt("Select account type (1 or 2): ");
        if (type != 1 && type != 2) {
            cout << "Invalid choice. Please enter 1 or 2." << endl;
        }
    } while (type != 1 && type != 2);

    // Use savings requirements by default and select current requirements for type 2.
    double required = SA_INITIAL_MIN;
    string typeName = "Savings Account";
    if (type == 2) {
        required = CA_INITIAL_MIN;
        typeName = "Current Account";
    }

    // Require enough opening money before rounding the accepted amount to cents.
    double deposit;
    do {
        deposit = readDouble("Initial deposit: ");
        if (deposit < required) {
            cout << "The initial deposit is not sufficient. A " << typeName
                 << " needs " << required << " or more." << endl;
        }
    } while (deposit < required);
    deposit = roundToCents(deposit);

    // Require a positive PIN containing exactly six digits.
    int newPin;
    do {
        newPin = readInt("Set a 6-digit PIN: ");
        if (newPin <= 0 || countDigits(newPin) != 6) {
            cout << "PIN must be a 6-digit number." << endl;
        }
    } while (newPin <= 0 || countDigits(newPin) != 6);

    // Save the completed form, initialize the balance, and activate the account.
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

    // Display the new account summary with monetary amounts shown to two decimals.
    cout << fixed << setprecision(2);
    cout << endl << "Account created successfully!" << endl;
    cout << "Account Name   : " << firstName << " " << middleName << " " << lastName << endl;
    cout << "Account Number : " << accountNumber << endl;
    cout << "Account Type   : " << accountTypeName() << endl;
    cout << "Balance        : PHP " << balance << endl;
    cout << "Please keep your account number and PIN safe." << endl;
}

// Show the account number and current balance after verifying access.
void balanceInquiry() {
    printHeader("BALANCE INQUIRY");
    // Return to the menu if the account or PIN check fails.
    if (!checkAccess()) {
        return;
    }
    cout << fixed << setprecision(2);
    cout << endl << "Account Number  : " << accountNumber << endl;
    cout << "Current Balance : PHP " << balance << endl;
}

// Validate and confirm an amount before adding it to the account balance.
void deposit() {
    printHeader("DEPOSIT");
    // Protect the deposit screen with the shared account and PIN checks.
    if (!checkAccess()) {
        return;
    }

    // Select and display the minimum deposit for this account type.
    double minimum = minimumTransaction();
    cout << fixed << setprecision(2);
    cout << endl << "Minimum deposit for a " << accountTypeName() << ": PHP " << minimum << endl;

    // Keep asking for an amount until it meets the minimum or the user cancels.
    double amount;
    do {
        amount = readDouble("Amount to deposit (or 0 to cancel): ");
        // Cancel the transaction without changing the balance when zero is entered.
        if (amount == 0) {
            cout << "Transaction cancelled." << endl;
            return;
        }
        // Reject amounts below the required minimum deposit.
        if (amount < minimum) {
            cout << "The entered amount is not valid. Please enter a valid amount (minimum PHP "
                 << minimum << ") or enter 0 to cancel." << endl;
        }
    } while (amount < minimum);

    // Round the accepted amount before displaying the confirmation question.
    amount = roundToCents(amount);
    cout << "You are about to deposit PHP " << amount << "." << endl;
    // Add money only after the customer confirms the deposit.
    if (confirm("Proceed with the deposit?")) {
        balance += amount;
        cout << endl << "Transaction successful! You deposited PHP " << amount << "." << endl;
        cout << "New balance: PHP " << balance << endl;
    } else {
        // Leave the balance unchanged if the customer declines confirmation.
        cout << "Transaction cancelled." << endl;
    }
}

// Validate and confirm an amount before subtracting it from the balance.
void withdraw() {
    printHeader("WITHDRAW");
    // Protect withdrawals with the same account and PIN checks.
    if (!checkAccess()) {
        return;
    }

    // Display the available balance and the minimum withdrawal amount.
    double minimum = minimumTransaction();
    cout << fixed << setprecision(2);
    cout << endl << "Current balance: PHP " << balance << endl;
    cout << "Minimum withdrawal for a " << accountTypeName() << ": PHP " << minimum << endl;

    // Repeat until the withdrawal is allowed or zero is entered to cancel.
    double amount;
    do {
        amount = readDouble("Amount to withdraw (or 0 to cancel): ");
        // Return to the menu without changing the balance when the user cancels.
        if (amount == 0) {
            cout << "Transaction cancelled." << endl;
            return;
        }
        // Reject withdrawals below the minimum or greater than the current balance.
        if (amount < minimum || amount > balance) {
            cout << "The entered amount is not valid. Please enter a valid amount (minimum PHP "
                 << minimum << ", not more than your balance) or enter 0 to cancel." << endl;
        }
    } while (amount < minimum || amount > balance);

    // Round the accepted amount and show what the user is about to withdraw.
    amount = roundToCents(amount);
    cout << "You are about to withdraw PHP " << amount << "." << endl;
    // Subtract money only after the customer confirms the withdrawal.
    if (confirm("Proceed with the withdrawal?")) {
        balance -= amount;
        cout << endl << "Transaction successful! You withdrew PHP " << amount << "." << endl;
        cout << "Remaining balance: PHP " << balance << endl;
    } else {
        // Preserve the balance when the customer declines the withdrawal.
        cout << "Transaction cancelled." << endl;
    }
}

// Display the stored customer and account information without changing it.
void viewAccountInformation() {
    printHeader("VIEW ACCOUNT INFORMATION");
    // Require account access before showing personal and financial details.
    if (!checkAccess()) {
        return;
    }
    // Format the account details and translate stored values into readable labels.
    cout << fixed << setprecision(2);
    cout << endl << "Account Number  : " << accountNumber << endl;
    cout << "Full Name       : " << firstName << " " << middleName << " " << lastName << endl;
    cout << "Address         : " << address << endl;
    cout << "Birthday        : " << monthName(birthMonth) << " " << birthDay << ", " << birthYear << endl;
    cout << "Gender          : ";
    // Display the word Male or Female from the stored gender character.
    if (gender == 'M') {
        cout << "Male" << endl;
    } else {
        cout << "Female" << endl;
    }
    cout << "Account Type    : " << accountTypeName() << endl;
    cout << "Initial Deposit : PHP " << initialDeposit << endl;
    cout << "Current Balance : PHP " << balance << endl;
}

// Display the thank-you screen and project group members.
void exitProgram() {
    printHeader("THANK YOU");
    cout << "Thank you for using our banking system!" << endl << endl;
    cout << "Group / Bank Name: " << BANK_NAME << endl;
    cout << "Members:" << endl;
    cout << "  - Iverson Fontanilla" << endl;
    cout << "  - Kian Hizon" << endl;
    cout << "  - Pablo Tirona" << endl;
    cout << "  - Manuel Roska" << endl;
    printLine('=');
}

// Display the six banking options; main handles the actual selection.
void showMenu() {
    printHeader("MAIN MENU");
    cout << "  1. Open a New Account" << endl;
    cout << "  2. Balance Inquiry" << endl;
    cout << "  3. Deposit" << endl;
    cout << "  4. Withdraw" << endl;
    cout << "  5. View Account Information" << endl;
    cout << "  6. Exit Program" << endl;
    printLine('-');
}

// Start the program and coordinate the menu and banking functions.
int main() {
    // Seed the random-number generator once using the current time.
    srand(static_cast<unsigned int>(time(0)));
    int choice;

    // Show the menu at least once and repeat until option 6 is selected.
    do {
        showMenu();
        choice = readInt("Select an option (1-6): ");

        // Call the banking function that matches the selected menu option.
        switch (choice) {
            // Option 1 collects information and opens a new account.
            case 1:
                openAccount();
                break;
            // Option 2 displays the balance after verifying access.
            case 2:
                balanceInquiry();
                break;
            // Option 3 allows a confirmed deposit.
            case 3:
                deposit();
                break;
            // Option 4 allows a confirmed withdrawal.
            case 4:
                withdraw();
                break;
            // Option 5 shows the stored account information.
            case 5:
                viewAccountInformation();
                break;
            // Option 6 displays the farewell screen before the menu loop ends.
            case 6:
                exitProgram();
                break;
            // Report menu choices that do not match any available option.
            default:
                cout << "Invalid option. Please enter a number from 1 to 6." << endl;
        }
    } while (choice != 6);

    // End main successfully after the user chooses to exit.
    return 0;
}
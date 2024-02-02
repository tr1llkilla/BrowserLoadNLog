/*
Copyright (C) 2023 tr1llkilla
    This program comes with ABSOLUTELY NO WARRANTY.
    This is free software, and you are welcome to redistribute it
    under legal conditions.
Author:
tr1llkilla

Author's note:
This program is unfinished, but does the trick to get the job done manually
Feel free to credit this work in any future registry forensic or data analysis works!
*/

#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <stdexcept>

#ifdef _WIN32
#include <Windows.h>
#endif

// Helper function to convert UTF-8 to wide string
std::wstring Utf8ToWide(const std::string& utf8Str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    std::wstring wideStr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], len);
    return wideStr;
}

// Base class for web browsers
class WebBrowser {
protected:
    std::stack<std::string> history;
    std::stack<std::string> forwardHistory;
    std::vector<std::string> pageLog;

public:
    virtual ~WebBrowser() = default;

    // Function to visit a new page
    virtual void visitPage(const std::string& page) {
        history.push(page);
        pageLog.push_back(page);
        while (!forwardHistory.empty()) {
            forwardHistory.pop();
        }
        std::cout << "Visited page: " << page << std::endl;
    }

    // Function to navigate back to the previous page
    virtual void navigateBack() {
        if (!history.empty()) {
            std::string current = history.top();
            forwardHistory.push(current);
            history.pop();
            std::cout << "Navigated back from: " << current << std::endl;
        }
        else {
            std::cout << "No more pages to go back to." << std::endl;
        }
    }

    // Function to navigate forward to the next page
    virtual void navigateForward() {
        if (!forwardHistory.empty()) {
            std::string next = forwardHistory.top();
            history.push(next);
            forwardHistory.pop();
            std::cout << "Navigated forward to: " << next << std::endl;
        }
        else {
            std::cout << "No forward history available." << std::endl;
        }
    }

    // Function to display the current page
    virtual void displayCurrentPage() const {
        if (!history.empty()) {
            std::cout << "Current page: " << history.top() << std::endl;
        }
        else {
            std::cout << "No pages visited yet." << std::endl;
        }
    }

    // Function to display the log of visited pages
    virtual void displayPageLog() const {
        std::cout << "Visited Pages:\n";
        for (const std::string& page : pageLog) {
            std::cout << "- " << page << std::endl;
        }
    }

    // Function to open a URL
    virtual void openURL(const std::string& url) const {
#ifdef _WIN32
        // Convert the URL to a wide string
        std::wstring wideUrl = Utf8ToWide(url);

        // Use ShellExecute to open the Brave browser with the specified path
        ShellExecuteW(NULL, L"open", L"C:\\Program Files\\BraveSoftware\\Brave-Browser\\Application\\brave.exe",
            wideUrl.c_str(), NULL, SW_SHOWNORMAL);
#else
        // Implement for other platforms if needed
        std::cout << "Opening URL: " << url << std::endl;
#endif
    }
};

// Derived class for BraveWebBrowser
class BraveWebBrowser : public WebBrowser {
public:
    void specialBraveFeature() {
        // Implement special features specific to BraveWebBrowser if needed
        std::cout << "Special feature for BraveWebBrowser!\n";
    }

    // Overridden function to open a URL (actual implementation for BraveWebBrowser)
    void openURL(const std::string& url) const override {
#ifdef _WIN32
        // Convert the URL to a wide string
        std::wstring wideUrl = Utf8ToWide(url);

        // Use ShellExecute to open the URL in Brave browser with the specified path
        ShellExecuteW(NULL, L"open", L"C:\\Program Files\\BraveSoftware\\Brave-Browser\\Application\\brave.exe",
            wideUrl.c_str(), NULL, SW_SHOWNORMAL);
#else
        // Implement for other platforms if needed
        std::cout << "Opening URL in BraveWebBrowser: " << url << std::endl;
#endif
    }
};

// Class to manage different web browsers
class BrowserManager {
private:
    std::vector<WebBrowser*> browsers;

public:
    ~BrowserManager() {
        // Clean up browser instances
        for (WebBrowser* browser : browsers) {
            delete browser;
        }
    }

    // Function to add a new browser to the manager
    void addBrowser(WebBrowser* browser) {
        browsers.push_back(browser);
    }
};

// Function to determine if a character is an operator
bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

// Function to perform arithmetic operations
int performOperation(int operand1, int operand2, char op) {
    switch (op) {
    case '+': return operand1 + operand2;
    case '-': return operand1 - operand2;
    case '*': return operand1 * operand2;
    case '/': return operand1 / operand2;
    default: throw std::invalid_argument("Invalid operator");
    }
}

// Function to convert infix expression to postfix
std::string infixToPostfix(const std::string& infix) {
    std::stringstream postfix;
    std::stack<char> operators;

    for (char c : infix) {
        if (std::isdigit(c)) {
            postfix << c << ' ';  // Output operands directly to postfix
        }
        else if (c == '(') {
            operators.push(c);
        }
        else if (c == ')') {
            // Pop operators from the stack and append to postfix until '(' is encountered
            while (!operators.empty() && operators.top() != '(') {
                postfix << operators.top() << ' ';
                operators.pop();
            }
            operators.pop();  // Pop '('
        }
        else if (isOperator(c)) {
            // Pop operators with higher or equal precedence from the stack and append to postfix
            while (!operators.empty() && isOperator(operators.top()) &&
                ((c != '*' && c != '/') || (operators.top() == '*' || operators.top() == '/'))) {
                postfix << operators.top() << ' ';
                operators.pop();
            }
            operators.push(c);
        }
    }

    // Pop any remaining operators from the stack and append to postfix
    while (!operators.empty()) {
        postfix << operators.top() << ' ';
        operators.pop();
    }

    return postfix.str();
}

// Function to evaluate postfix expression
int evaluatePostfix(const std::string& postfix) {
    std::stack<int> operands;
    std::stringstream ss(postfix);
    std::string token;

    while (ss >> token) {
        if (std::isdigit(token[0])) {
            operands.push(std::stoi(token));
        }
        else if (isOperator(token[0])) {
            // Perform operation with the top two operands on the stack
            int operand2 = operands.top();
            operands.pop();
            int operand1 = operands.top();
            operands.pop();
            operands.push(performOperation(operand1, operand2, token[0]));
        }
        else {
            throw std::invalid_argument("Invalid token in postfix expression");
        }
    }

    if (operands.size() != 1) {
        throw std::invalid_argument("Invalid postfix expression");
    }

    return operands.top();
}

int main() {
    BrowserManager browserManager;

    // Create and add instances of different browsers
    WebBrowser* defaultBrowser = new WebBrowser();
    BraveWebBrowser* braveBrowser = new BraveWebBrowser();

    browserManager.addBrowser(defaultBrowser);
    browserManager.addBrowser(braveBrowser);

    while (true) {
        std::cout << "\nMenu:\n";
        std::cout << "1. Visit Page\n";
        std::cout << "2. Navigate Back\n";
        std::cout << "3. Navigate Forward\n";
        std::cout << "4. Display Current Page\n";
        std::cout << "5. Evaluate Expression\n";
        std::cout << "6. Display Page Log\n";
        std::cout << "7. BraveWebBrowser Special Feature\n";  // New option for special feature
        std::cout << "8. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Consume the newline character

        switch (choice) {
        case 1: {
            std::cout << "Enter page name: ";
            std::string page;
            std::getline(std::cin, page);
            defaultBrowser->visitPage(page);
            break;
        }
        case 2:
            defaultBrowser->navigateBack();
            break;
        case 3:
            defaultBrowser->navigateForward();
            break;
        case 4:
            defaultBrowser->displayCurrentPage();
            break;
        case 5: {
            std::cout << "Enter an arithmetic expression: ";
            std::string infixExpression;
            std::getline(std::cin, infixExpression);

            try {
                std::string postfixExpression = infixToPostfix(infixExpression);
                std::cout << "Postfix expression: " << postfixExpression << std::endl;

                int result = evaluatePostfix(postfixExpression);
                std::cout << "Result: " << result << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }

            break;
        }
        case 6:
            defaultBrowser->displayPageLog();
            break;
        case 7:
            // Special feature for BraveWebBrowser
            if (dynamic_cast<BraveWebBrowser*>(defaultBrowser)) {
                braveBrowser->specialBraveFeature();
            }
            else {
                std::cout << "Special feature not available for this browser.\n";
            }
            break;
        case 8:
            std::cout << "Exiting the program.\n";
            return 0;
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}

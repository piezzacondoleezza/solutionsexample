#include <iostream>
#include <string>
#include <cctype>
#include <string_view>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <stdexcept>

// Program can evaluate expressions containing symbols: 0-9+-*/()., 


enum SymbolType {NUMBER, 
        OPEN_BRACKET, 
        CLOSE_BRACKET,
        OPERATION,
        TRASH
        };


namespace ExpressionCorrectness {

    bool IsDigit(char c) noexcept {
        return isdigit(c);
    }

    bool IsDigitDelimeter(char c) noexcept {
        return c == '.' || c == ',';
    }

    bool IsLowPriorityOperation(char c) noexcept {
        return c == '+' || c == '-';
    }

    bool IsHighPriorityOperation(char c) noexcept {
        return c == '*' || c == '/';
    }

    bool IsOperation(char c) noexcept {
        return IsLowPriorityOperation(c) || IsHighPriorityOperation(c);
    }

    bool IsBracket(char c) noexcept {
        return c == ')' || c == '(';
    }

    bool CorrectSymbol(char c) noexcept {
        return IsDigit(c) || IsOperation(c) || IsBracket(c) || c == ' ' || IsDigitDelimeter(c);
    }

    bool CheckSymbolsCorrectness(const std::string& expression) noexcept {
        for (size_t i = 0; i < expression.size(); ++i) {
            if (!CorrectSymbol(expression[i])) {
                return false;
            }
        }
        return true;
    }

    SymbolType FindNextSymbol(const std::string& expression, size_t index) {
        for (size_t i = index; i < expression.size(); ++i) {
            if (expression[i] == '(') {
                return OPEN_BRACKET;
            } else if (expression[i] == ')') {
                return CLOSE_BRACKET;
            } else if (IsOperation(expression[i])) {
                return OPERATION;
            } else if (IsDigit(expression[i])) {
                return NUMBER;
            }
        }
        return CLOSE_BRACKET; // assuming that expression ends with virtual close bracket
    }

    bool CheckBalanceCorrectness(const std::string& expression) noexcept {
        int balance = 0;
        for (size_t i = 0; i < expression.size(); ++i) {
            if (expression[i] == '(') {
                ++balance;
            } else if (expression[i] == ')') {
                --balance;
            }
            if (balance < 0) {
                return false;
            }
        }
        return balance == 0;
    }

    bool CheckOrderCorrectness(const std::string& expression) noexcept {
        SymbolType previousSymbol = OPEN_BRACKET; // assuming that we start expression with virtual open bracket
        for (size_t i = 0; i < expression.size(); ++i) {
            if (expression[i] == ' ') {
                continue;
            }
            if (IsDigitDelimeter(expression[i])) {
                return false;
            } else if (IsLowPriorityOperation(expression[i])) {
                if (previousSymbol == OPERATION || FindNextSymbol(expression, i) == CLOSE_BRACKET) {
                    return false;
                }
                previousSymbol = OPERATION;
            } else if (IsHighPriorityOperation(expression[i])) {
                if (previousSymbol == OPERATION) {
                    return false;
                }
                if ((previousSymbol == OPEN_BRACKET && FindNextSymbol(expression, i + 1) == NUMBER) || 
                    FindNextSymbol(expression, i + 1) == CLOSE_BRACKET) {
                        return false;
                }
                previousSymbol = OPERATION;
            } else if (IsBracket(expression[i])) {
                if (expression[i] == '(') {
                    if (previousSymbol == CLOSE_BRACKET) {
                        return false;
                    }
                    previousSymbol = OPEN_BRACKET;
                } else {
                    if (previousSymbol == OPERATION || previousSymbol == OPEN_BRACKET) { // forbidding empty constructions like ()
                        return false;
                    }
                    previousSymbol = CLOSE_BRACKET;
                }
            } else {  // Symbol is a digit
                if (previousSymbol == CLOSE_BRACKET || previousSymbol == NUMBER) {
                    return false;
                }
                size_t j = i;
                size_t delimeters = 0;
                while (j < expression.size() && (IsDigit(expression[j]) || IsDigitDelimeter(expression[j]))) {
                    delimeters += static_cast<size_t>(IsDigitDelimeter(expression[j]));
                    ++j;
                }
                if (delimeters > 1) {
                    return false;
                }
                previousSymbol = NUMBER;
                i = j - 1;
            }
        }
        return true;
    }
    
};

class Expression {
public:
    Expression(const std::string& s) : expression(s) {}
    
    Expression(std::string&& s) : expression(s) {}

    void transformToUniversal() {
        replace(expression.begin(), expression.end(), ',', '.');
        expression.erase(remove(expression.begin(), expression.end(), ' '), expression.end());
        return;
    }

    bool CheckCorrectness() const noexcept {
        bool correct = true;
        correct &= ExpressionCorrectness::CheckBalanceCorrectness(expression);
        correct &= ExpressionCorrectness::CheckSymbolsCorrectness(expression);
        correct &= ExpressionCorrectness::CheckOrderCorrectness(expression);
        return correct;
    }
    
    double Evaluate() const {
        return Evaluate(expression);
    }

    double Evaluate(const std::string& expr) const {
        std::string transformed = "";
        // cout << expr << endl;
        for (size_t i = 0; i < expr.size(); ++i) {
            if (expr[i] == '(') {
                size_t j = i;
                size_t balance = 0;
                while (balance != 0 || j == i) {
                    if (expr[j] == '(') {
                        ++balance;
                    } else if (expr[j] == ')') {
                        --balance;
                    }
                    ++j;
                }
                transformed += std::to_string(Evaluate(expr.substr(i + 1, j - i - 2)));
                i = j;
            }
            transformed += expr[i];
        }
        for (int i = static_cast<int>(transformed.size()) - 1; i >= 0; --i) {
            if (transformed[i] == '+') {
                return Evaluate(transformed.substr(0, i)) + Evaluate(transformed.substr(i + 1, static_cast<int>(transformed.size()) - i));
            } else if (transformed[i] == '-') {
                return Evaluate(transformed.substr(0, i)) - Evaluate(transformed.substr(i + 1, static_cast<int>(transformed.size()) - i));
            }
        }
        for (int i = static_cast<int>(transformed.size()) - 1; i >= 0; --i) {
            if (transformed[i] == '*') {
                return Evaluate(transformed.substr(0, i)) * Evaluate(transformed.substr(i + 1, static_cast<int>(transformed.size()) - i));
            } else if (transformed[i] == '/') {
                return Evaluate(transformed.substr(0, i)) / Evaluate(transformed.substr(i + 1, static_cast<int>(transformed.size()) - i));
            }
        }
        if (transformed.size() == 0) {
            return 0;
        }
        return stod(transformed);
    }

    std::string GetExpression() const noexcept {
        return expression;
    }

    void Print() const noexcept {
        std::cout << expression << '\n';
        return;
    }

private:
    std::string expression;
};

namespace UnitTests {

    template<class T>
    void AssertEqual(T a, T b) {
        assert(a == b);
    }

    template<class lambda>
    void TestOrderCorrectness(const std::string&& test, bool result, lambda& f) {
        AssertEqual(f(test), result);
    }

    template<class lambda>
    void TestFunctionCorrectness(const std::string& test, bool result, lambda& f) {
        AssertEqual(f(test), result);
    }

    void TestEvaluation(Expression&& expression, double result) {
        const double eps = 1e-3;
        expression.transformToUniversal();
        assert(fabs(expression.Evaluate() - result) <= eps);
    }

    void TestOrderCorrectness() {
        auto orderCorrectness = [](const std::string& expression) {
            return ExpressionCorrectness::CheckOrderCorrectness(expression);
        };

        TestFunctionCorrectness("()", false, orderCorrectness);
        TestFunctionCorrectness("(+*)", false, orderCorrectness);
        TestFunctionCorrectness("(+ 2*)  ", false, orderCorrectness);
        TestFunctionCorrectness("(*3+5)", false, orderCorrectness);
        TestFunctionCorrectness("(2)(3)", false, orderCorrectness);
        TestFunctionCorrectness("2.0 3", false, orderCorrectness);
        TestFunctionCorrectness("2,0.5 3.0", false, orderCorrectness);

        TestFunctionCorrectness("2+3*(7*5*(-1))", true, orderCorrectness);
        TestFunctionCorrectness("2+3/(7*5*(-1))", true, orderCorrectness);
        TestFunctionCorrectness("((2)*(-7)/228.0+1337,0)", true, orderCorrectness);
        TestFunctionCorrectness("((2)*(-7)/228+(((1337,24))))", true, orderCorrectness);
    }

    void TestBalanceCorrectness() {
        auto balanceCorrectness = [](const std::string expression) {
            return ExpressionCorrectness::CheckBalanceCorrectness(expression);
        };

        TestFunctionCorrectness("(()", false, balanceCorrectness);
        TestFunctionCorrectness("(", false, balanceCorrectness);
        TestFunctionCorrectness("))  ))    ))", false, balanceCorrectness);
        TestFunctionCorrectness("((  ))) (((    ))", false, balanceCorrectness);
        TestFunctionCorrectness("((  )) (((    ))", false, balanceCorrectness);
        TestFunctionCorrectness(")", false, balanceCorrectness);

        TestFunctionCorrectness("()", true, balanceCorrectness);
        TestFunctionCorrectness("()((()))    ()   ()    (())", true, balanceCorrectness);
        TestFunctionCorrectness("         ", true, balanceCorrectness);
    }

    void TestSymbolCorrectness() {
        auto symbolCorrectness = [](const std::string expression) {
            return ExpressionCorrectness::CheckSymbolsCorrectness(expression);
        };

        TestFunctionCorrectness("trash", false, symbolCorrectness);
        TestFunctionCorrectness("(()) a)", false, symbolCorrectness);
        TestFunctionCorrectness("//aa228.0", false, symbolCorrectness);

        TestFunctionCorrectness("()()()   , ,.,., ,,. , 42154 ()()2423415421.0", true, symbolCorrectness);
        TestFunctionCorrectness("342143214 (( *+-**/-( 21.0", true, symbolCorrectness);
    }

    void TestEvaluation() {
        TestEvaluation(Expression("-(2*7*10+10/(1+1))"), -145);
        TestEvaluation(Expression("+2+3-5+7/3+4/3+5*(7+1)  /10 / 20 + 55.23 - 47,28"), 11.816);
        TestEvaluation(Expression("-(-(-10))"), -10);
        TestEvaluation(Expression("10+20+30+40+17*17/10+3/14+2/7 + 2   /(6+4*(3/(7+5.23*(8+5,7)*10,5)))*3.4"), 130.53);
        TestEvaluation(Expression("-1 + 5 - 3"), 1);
        TestEvaluation(Expression("-10 + (8 * 2.5) - (3 / 1,5)"), 8);
        TestEvaluation(Expression("1 + (2 * (2.5 + 2.5 + (3 - 2))) - (3 / 1.5)"), 11);
        TestEvaluation(Expression("01 + 01 + 01 + 001"), 4);
        TestEvaluation(Expression("1488 / 228.22 +    1337*(47/2)"), 31426.02);
    }

    void RunUnitTests() {
        TestOrderCorrectness();
        TestBalanceCorrectness();
        TestSymbolCorrectness();
        TestEvaluation();
    }
    
};



int main() {
    UnitTests::RunUnitTests();
    std::string _expression;
    getline(std::cin, _expression);
    Expression expression(_expression);
    if (!expression.CheckCorrectness()) {
        std::cout << "Incorrect expression\n";
        return 0;
    }
    expression.transformToUniversal();
    expression.Print();
    double ans = 0;
    try {
        ans = expression.Evaluate();
    } catch (...) {
        std::cout << "Unexpected error" << std::endl;
    }
    std::cout << std::fixed << std::setprecision(2) << " result = " << ans << '\n';
    return 0;
}

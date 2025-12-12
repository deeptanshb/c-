
#include <iostream>
#include <regex>

bool isValidAssignment(const std::string& str) {
    // Regular expression to match a valid assignment statement
    std::regex re(R"(^[a-zA-Z]+\s*=\s*([a-zA-Z0-9+\-*/\s]*)$)");
    return std::regex_match(str, re);
}

int main() {
    std::string testStrings[] = {
        "a = b+c",
        "b = cd-",
        "a-==a+b",
        "a == 10",
        "a = a-10"
    };
   
    for (const auto& str : testStrings) {
        if (isValidAssignment(str)) {
            std::cout << str << " is a valid assignment statement.\n";
        } else {
            std::cout << str << " is not a valid assignment statement.\n";
        }
    }
   
    return 0;
}

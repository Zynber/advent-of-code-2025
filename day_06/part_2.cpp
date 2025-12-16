#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

long power(long base, long exp) {
  long result = 1;

  while (exp > 0) {
    if (exp & 1) result *= base;

    base *= base;
    exp >>= 1;
  }

  return result;
}

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<std::string> lines;

  while (std::getline(file, line))
    lines.emplace_back(line);

  long total = 0;
  std::vector<int> terms;

  for (int col = lines.back().size() - 1; col >= 0; --col) {
    std::vector<char> digits;
    int term = 0;

    for (auto it = std::next(lines.rbegin()); it != lines.rend(); ++it) {
      const char c = (*it)[col];

      if (c != ' ') digits.emplace_back(c);
    }

    // Ignore separator column
    if (!digits.size()) continue;

    for (int i = 0; i < (int) digits.size(); ++i)
      term += (digits[i] - '0') * power(10, i);

    terms.emplace_back(term);

    const char operand = lines.back()[col];

    if (operand == ' ') continue;

    long result = operand == '*' ? 1 : 0;

    for (auto term : terms)
      result = operand == '*' ? result * term : result + term;

    total += result;
    terms.clear();
  }

  std::cout << total << std::endl;
  return 0;
}

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::vector<std::vector<int>> terms;
  std::vector<char> operands;

  while (std::getline(file, line)) {
    if (line.front() == '*' || line.front() == '+') break;

    std::istringstream iss(line);
    int term;
    terms.emplace_back();

    while (iss >> term)
      terms.back().emplace_back(term);
  }

  for (char c : line)
    if (c != ' ') operands.emplace_back(c);

  long total = 0;

  for (int i = 0; i < (int) operands.size(); ++i) {
    char operand = operands[i];
    long result = operand == '*' ? 1 : 0;

    for (auto term : terms)
      result = operand == '*' ? result * term[i] : result + term[i];

    total += result;
  }

  std::cout << total << std::endl;
  return 0;
}

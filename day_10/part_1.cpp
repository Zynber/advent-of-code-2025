#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

void parse_input(
  std::string &line,
  int &width,
  std::bitset<10> &goal_state,
  std::vector<std::bitset<10>> &buttons
) {
  std::vector<std::string> tokens;
  std::istringstream iss(line);
  std::string token;
  std::getline(iss, token, ' ');
  width = token.size() - 2;

  for (int i = 1; i < (int) token.size() - 1; ++i)
    if (token[i] == '#') goal_state.set(i - 1);

  while (std::getline(iss, token, ' ')) {
    if (token[0] == '{') break;

    auto &bitset = buttons.emplace_back();

    for (int j = 1; j < (int) token.size(); j += 2)
      bitset.set(token[j] - '0');
  }
}

int main() {
  std::fstream file("input.txt");
  std::string line;
  int sum = 0;

  while (std::getline(file, line)) {
    int width;
    std::bitset<10> goal_state;
    std::vector<std::bitset<10>> buttons;
    parse_input(line, width, goal_state, buttons);

    std::unordered_map<std::bitset<10>, int> possible_states{{0, 0}};
    std::unordered_set<std::bitset<10>> prev_states{0};
    int iter_count = 1;

    while (!possible_states.contains(goal_state)) {
      std::unordered_set<std::bitset<10>> curr_states;

      for (auto &prev_state : prev_states) {
        for (auto button : buttons) {
          auto curr_state = prev_state ^ button;
          auto [_, ok] = possible_states.emplace(curr_state, iter_count);

          if (ok) curr_states.emplace(curr_state);
        }
      }

      prev_states = std::move(curr_states);
      ++iter_count;
    }

    sum += possible_states.at(goal_state);
  }

  std::cout << sum << std::endl;
  return 0;
}

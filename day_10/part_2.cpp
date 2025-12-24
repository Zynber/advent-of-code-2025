#include <algorithm>
#include <fstream>
#include <generator>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <valarray>
#include <vector>

using namespace std::literals;

std::string stringify_button(const std::valarray<int> &v) {
  return std::string("(")
    + (std::views::enumerate(v)
    | std::views::filter([](const auto &t) { return std::get<1>(t); })
    | std::views::transform([](const auto &t) {
        return std::to_string(std::get<0>(t));
      })
    | std::views::join_with(',')
    | std::ranges::to<std::string>())
    + ")";
}

template <class R>
  requires std::ranges::viewable_range<R>
  && std::convertible_to<std::ranges::range_reference_t<R>, int>
std::string stringify_ints(R &&r) {
  return std::format(
    "({})",
    r
      | std::views::transform([](const auto &c) { return std::to_string(c); })
      | std::views::join_with(", "sv)
      | std::ranges::to<std::string>()
  );
}

void parse_input(
  const std::string &line,
  int &width,
  std::vector<std::vector<std::valarray<int> *>> &button_map,
  std::valarray<int> &count,
  std::valarray<int> &goal_joltages
) {
  std::vector<std::string> tokens;
  std::istringstream iss(line);
  std::string token;
  std::getline(iss, token, ' ');
  width = token.size() - 2;
  button_map = std::vector<std::vector<std::valarray<int> *>>(width);

  std::vector<std::valarray<int> *> buttons;

  while (std::getline(iss, token, ' ')) {
    if (token[0] == '{') break;

    auto v = buttons.emplace_back(new std::valarray<int>(width));

    for (int i = 1; i < (int) token.size(); i += 2) (*v)[token[i] - '0'] = 1;
  }

  for (auto button : buttons)
    for (int i = 0; i < width; ++i)
      if ((*button)[i]) button_map[i].emplace_back(button);

  count = std::accumulate(
    buttons.begin(),
    buttons.end(),
    std::valarray<int>(width),
    [](std::valarray<int> a, std::valarray<int> *b) { return a += *b; }
  );

  // Prioritize buttons that affect more joltages
  for (auto &buttons : button_map)
    std::sort(
      buttons.begin(),
      buttons.end(),
      [](std::valarray<int> *a, std::valarray<int> *b) {
        return a->sum() > b->sum();
      }
    );

  std::istringstream sub_iss(token.substr(1, token.size() - 2));
  std::string sub_token;

  goal_joltages = std::valarray<int>(width);
  int i = 0;

  while (std::getline(sub_iss, sub_token, ','))
    goal_joltages[i++] = std::stoi(sub_token);
}

std::generator<std::vector<int>> generate_allocation(
  int idx,
  int n,
  int k,
  const std::vector<int> &upper_bounds,
  // Most units that can be assigned to consumers right of i
  const std::vector<int> &upper_bounds_right_sum,
  std::vector<int> &alloc
) {
  // Base case
  if (idx == k - 1) {
    if (n <= upper_bounds[idx]) {
      alloc[idx] = n;
      co_yield alloc;
    }

    co_return;
  }

  int min = std::max(0, n - upper_bounds_right_sum[idx]);
  int max = std::min(n, upper_bounds[idx]);

  for (int i = max; i >= min; --i) {
    alloc[idx] = i;

    for (
      const auto &&a : generate_allocation(
        idx + 1, n - i, k, upper_bounds, upper_bounds_right_sum, alloc
      )
    )
      co_yield a;
  }
}

// Generate all the ways that n items can be distributed to k consumers
// First consumers are prioritized over last consumers
std::generator<std::vector<int>> generate_allocation(
  int n, std::vector<int> upper_bounds
) {
  const int k = upper_bounds.size();
  std::vector<int> alloc(k), upper_bounds_right_sum(k);

  {
    int sum = 0;

    for (int i = upper_bounds.size() - 1; i >= 0; --i) {
      upper_bounds_right_sum[i] = sum;
      sum += upper_bounds[i];
    }

    if (sum < n) co_return;
  }

  for (
    auto &&a :
    generate_allocation(0, n, k, upper_bounds, upper_bounds_right_sum, alloc)
  )
    co_yield a;
}

void solve(
  const std::vector<std::vector<std::valarray<int> *>> &button_map,
  const std::valarray<int> &remaining_joltages,
  const std::vector<int> &button_influence_map,
  int &known_min,
  long &inspect_count,
  long &skip_count,
  int cumulative_cost = 0,
  int heuristic_idx = 0
) {
  ++inspect_count;

  if (!remaining_joltages.sum()) {
    std::cout
      << std::format(
           "\nSolver: found upper bound {} at inpsection {}",
           cumulative_cost,
           inspect_count
         )
      << std::endl;
    known_min = std::min(known_min, cumulative_cost);
    return;
  }

  // Instead of a static heuristic (see line 363), choose the next joltage to
  // recurse dynamically
  // I terribly overestimated the cost of computing this on-the-fly, or
  // underestimated how powerful a good heuristic is :'[
  // This was the FINAL change that made it acceptably fast (still not fast by
  // any means, compile with -O3 lol)
  const auto range = remaining_joltages
    | std::views::enumerate
    // Ignore joltages with 0
    | std::views::filter([](const auto &t) -> bool { return std::get<1>(t); })
    | std::views::transform([&](const auto &t) {
        const auto &buttons = button_map[std::get<0>(t)];
        auto view = buttons
          // Maximum time each button can be pressed without causing any
          // component of remaining_joltages to become < 0
          | std::views::transform([&](const auto &button) {
              return std::ranges::min(
                std::views::enumerate(*button)
                | std::views::filter([&](const auto &t) {
                    return std::get<1>(t);
                  })
                | std::views::transform([&](const auto &t) {
                    return remaining_joltages[std::get<0>(t)];
                  })
              );
            })
          | std::views::enumerate
          // Remove those where the maximum time is 0
          | std::views::filter([&](const auto &t) { return std::get<1>(t); });

        std::vector<std::valarray<int> *> applicable_buttons;
        std::vector<int> upper_bounds;

        applicable_buttons.reserve(buttons.size());
        upper_bounds.reserve(buttons.size());

        for (const auto &&t : view) {
          applicable_buttons.emplace_back(buttons[std::get<0>(t)]);
          upper_bounds.emplace_back(std::get<1>(t));
        }

        return std::tuple{
          std::get<0>(t), // target_joltage_idx
          std::get<1>(t), // target_remaining_joltage
          applicable_buttons,
          upper_bounds
        };
      })
    | std::views::filter([](const auto &t) -> bool {
        return std::get<2>(t).size();
      })
    | std::ranges::to<std::vector>();

  if (!range.size()) return;

  const auto
    [target_joltage_idx,
      target_remaining_joltage,
      applicable_buttons,
      upper_bounds] =
      std::ranges::min(range, [](const auto &a, const auto &b) -> bool {
        const auto a_size = std::get<2>(a).size(),
                   b_size = std::get<2>(b).size();
        return a_size == b_size ? std::get<1>(a) > std::get<1>(b)
                                : a_size < b_size;
      });

  const int next_cumulative_cost = cumulative_cost + target_remaining_joltage;

  if (next_cumulative_cost >= known_min) {
    ++skip_count;
    return;
  }

  for (
    const auto &alloc :
    generate_allocation(target_remaining_joltage, upper_bounds)
  ) {
    if (skip_count && !(skip_count % 1000000)) {
      std::cout
        << "\rSolver: Skipped "
        << skip_count
        << " times so far... (cumulative_cost="
        << cumulative_cost
        << ", target_joltage_idx="
        << target_joltage_idx
        << ", applicable_buttons=["
        << (applicable_buttons
             | std::views::transform([](const auto &button) {
                 return stringify_button(*button);
               })
             | std::views::join_with(", "sv)
             | std::ranges::to<std::string>())
        << "], alloc="
        << stringify_ints(alloc)
        << ")";
      std::cout.flush();
    }

    const std::valarray<int> next_remaining_joltages = remaining_joltages
      - std::ranges::fold_left(
        std::views::zip_transform(
          [](const auto &button, const auto &click_count) {
            return (*button) * click_count;
          },
          applicable_buttons,
          alloc
        ),
        std::valarray<int>(remaining_joltages.size()),
        std::plus<std::valarray<int>>()
      );

    const bool overjoltage = std::ranges::any_of(
      next_remaining_joltages, [](const int &i) { return i < 0; }
    );

    if (overjoltage) {
      ++skip_count;
      continue;
    }

    if (known_min != std::numeric_limits<int>::max()) {
      const int button_influence = button_influence_map[target_joltage_idx];
      const int lower_bound = cumulative_cost
        + std::max(
          next_remaining_joltages.max(),
          // Integer division, but rounded up
          (next_remaining_joltages.sum() + (button_influence - 1))
            / button_influence
        );

      if (lower_bound >= known_min) {
        ++skip_count;
        continue;
      }
    }

    solve(
      button_map,
      next_remaining_joltages,
      button_influence_map,
      known_min,
      inspect_count,
      skip_count,
      next_cumulative_cost,
      heuristic_idx + 1
    );
  }
}

int solve(
  const std::vector<std::vector<std::valarray<int> *>> &button_map,
  const std::valarray<int> &remaining_joltages,
  const std::vector<int> &button_influence_map,
  long &inspect_count,
  long &skip_count
) {
  int known_min = std::numeric_limits<int>::max();

  solve(
    button_map,
    remaining_joltages,
    button_influence_map,
    known_min,
    inspect_count,
    skip_count
  );

  return known_min;
}

int main() {
  std::fstream file("input.txt");
  std::ofstream out("output.txt");
  std::string line;
  int sum = 0, line_count = 1;

  while (std::getline(file, line)) {
    int width;
    std::vector<std::vector<std::valarray<int> *>> button_map;
    std::valarray<int> count, goal_joltages;
    parse_input(line, width, button_map, count, goal_joltages);

    // Joltage indices sorted by number of buttons that increment the joltage,
    // ascending
    // Break ties by prioritizing the one with the higher joltage
    // Edit: SO THIS WAS A HORRIBLE IDEA
    // std::vector<int> heuristics(width);
    // std::iota(heuristics.begin(), heuristics.end(), 0);
    // std::sort(heuristics.begin(), heuristics.end(), [&](int a, int b) {
    //   return count[a] == count[b] ? goal_joltages[a] > goal_joltages[b]
    //                               : count[a] < count[b];
    // });
    std::cout << std::string(80, '=') << "\nLINE " << line_count << std::endl;

    // For each joltage index, from the buttons that affect it, find the one
    // that affect the most joltages, then get the number of joltages it affects
    const auto button_influence_map = button_map
      | std::views::transform([](const auto &buttons) {
          return std::ranges::max(
            buttons | std::views::transform([](const auto &button) {
              return button->sum();
            })
          );
        })
      | std::ranges::to<std::vector<int>>();

    long inspect_count = 0, skip_count = 0;
    const int press_count = solve(
      button_map, goal_joltages, button_influence_map, inspect_count, skip_count
    );

    std::string out_str = std::format(
      "\nLINE {}: Found {} after {} inspections and {} skips",
      line_count++,
      press_count,
      inspect_count,
      skip_count
    );

    std::cout << out_str << std::endl;
    out << out_str << std::endl;
    sum += press_count;
  }

  std::cout << std::string(80, '=') << "\nSum: " << sum << std::endl;
  out << std::string(80, '=') << "\nSum: " << sum << std::endl;
  return 0;
}

// This solution with my input took 3m 26.350s to run, even when compiled with
// -O3 -march=native
// Worst line took 28,901,151 recursive calls and 80,515,194 prunes
// Whatever you do do NOT look at the commit date gap between day 9 and day 10

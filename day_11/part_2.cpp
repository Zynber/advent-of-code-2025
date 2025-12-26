#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Node {
  const std::string key;
  std::vector<Node *> to;
  int first_traversed = -1, last_traversed = -1;
  long incoming_path_count = 0;
  bool queued = false;

  Node(std::string key) : key(key) {}

  bool traversed() { return first_traversed >= 0; }
};

void reset_traversal_data(
  const std::unordered_map<std::string, Node *> &nodes
) {
  for (auto [_, node] : nodes) {
    node->first_traversed = node->last_traversed = -1;
    node->incoming_path_count = 0;
    node->queued = false;
  }
}

void log_layer(
  int curr_layer,
  int distinct_nodes,
  int repeated_nodes,
  int dest_nodes,
  const std::string &dest_name
) {
  std::cout
    << "Layer "
    << curr_layer
    << ": "
    << distinct_nodes
    << " distinct sub-nodes, "
    << repeated_nodes
    << " repeated sub-nodes";

  if (dest_nodes) std::cout << ", " << dest_nodes << " nodes to " << dest_name;

  std::cout << std::endl;
}

// Find when each node would be first and last explored by which iteration of
// BFS
void explore_bfs(Node *const start, const Node *const dest) {
  std::cout << "Exploring from " << start->key << "..." << std::endl;
  std::queue<Node *> q;
  std::unordered_set<std::string> nodes_queued_this_layer;
  int curr_layer = 0, distinct_nodes = 0, repeated_nodes = 0, dest_nodes = 0;
  q.emplace(start);
  start->first_traversed = start->last_traversed = 0;

  while (!q.empty()) {
    Node *&curr = q.front();
    q.pop();

    if (curr->first_traversed > curr_layer) {
      log_layer(
        curr_layer, distinct_nodes, repeated_nodes, dest_nodes, dest->key
      );
      ++curr_layer;
      distinct_nodes = repeated_nodes = dest_nodes = 0;
      nodes_queued_this_layer.clear();
    }

    for (const auto node : curr->to) {
      if (node->traversed()) {
        ++repeated_nodes;
        node->last_traversed = curr_layer + 1;
      } else {
        node->first_traversed = node->last_traversed = curr_layer + 1;
        ++distinct_nodes;
      }

      if (node == dest) ++dest_nodes;

      const auto [_, inserted] = nodes_queued_this_layer.emplace(node->key);

      if (inserted) q.emplace(node);
    }
  }

  log_layer(curr_layer, distinct_nodes, repeated_nodes, dest_nodes, dest->key);
}

struct NodeComparator {
  bool operator()(const Node *const a, const Node *const b) {
    return a->last_traversed > b->last_traversed;
  }
};

long count_paths(
  const std::unordered_map<std::string, Node *> &nodes,
  Node *const start,
  Node *const dest,
  long incoming_paths_to_start = 1
) {
  reset_traversal_data(nodes);
  explore_bfs(start, dest);
  std::priority_queue<Node *, std::vector<Node *>, NodeComparator> pq;
  pq.emplace(start);
  start->incoming_path_count = incoming_paths_to_start;

  while (!pq.empty()) {
    Node *curr = pq.top();
    pq.pop();

    for (Node *node : curr->to) {
      node->incoming_path_count += curr->incoming_path_count;

      if (!node->queued) {
        node->queued = true;
        pq.emplace(node);
      }
    }
  }

  return dest->incoming_path_count;
}

int main() {
  std::fstream file("input.txt");
  std::string line;
  std::unordered_map<std::string, Node *> nodes;

  while (std::getline(file, line)) {
    std::string key = line.substr(0, 3);
    const auto [pair, _] = nodes.try_emplace(key, new Node(key));
    std::istringstream iss(line.substr(5));
    std::string connected_node_key;

    while (iss >> connected_node_key) {
      const auto [connected_pair, _] =
        nodes.try_emplace(connected_node_key, new Node(connected_node_key));
      pair->second->to.emplace_back(connected_pair->second);
    }
  }

  Node *start = nodes.at("svr"), *dest = nodes.at("fft");
  const long svr_to_fft = count_paths(nodes, start, dest);
  std::cout << "svr -> fft: " << svr_to_fft << std::endl;

  start = dest;
  dest = nodes.at("dac");
  const long fft_to_dac = count_paths(nodes, start, dest, svr_to_fft);
  std::cout << "fft -> dac: " << fft_to_dac << std::endl;

  start = dest;
  dest = nodes.at("out");
  const long dac_to_out = count_paths(nodes, start, dest, fft_to_dac);
  std::cout << "dac -> out: " << dac_to_out << std::endl;

  std::cout << "Total paths: " << dac_to_out << std::endl;
  return 0;
}

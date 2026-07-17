#include "node.h"
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <node_id> <cluster_config_path> <input_data_path>\n";
        return 1;
    }

    int node_id = std::stoi(argv[1]);
    std::string config_file = argv[2];
    std::string data_file = argv[3];

    std::cout << "Initializing Node " << node_id << "...\n";
    Node node(node_id, config_file);

    // sleep to ensure peer nodes are ready to accept connections
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "Node " << node_id << " starting ingestion from: " << data_file << "\n";
    node.load_file(data_file);

    std::cout << "Node " << node_id << " load job complete. Broadcasting DONE signal...\n";
    node.broadcast_done();

    std::cout << "Node " << node_id << " waiting for synchronization barriers...\n";
    node.wait_for_completion();

    node.print_stats();

    return 0;
}
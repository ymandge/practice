#pragma once
#include "nodeconf.h"
#include "datastore.h"

#include <vector>
#include <string>
#include <thread>
#include <atomic>

class Node
{
    int node_id;
    int total_nodes;
    std::vector<NodeConfig> cluster;
    Datastore db;

    // stat variables
    std::atomic<int> records_read{0};
    std::atomic<int> records_stored{0};
    std::atomic<int> records_sent{0};
    std::atomic<int> records_received{0};
    std::atomic<int> done_signals_received{0};

    int server_fd{-1};
    std::atomic<bool> running{true};
    std::thread server_thread;

    bool send_message(const std::string& ip, int port, const std::string& msg);
    void handle_client(int client_sock);
    void run_server();
    void load_config(const std::string& filename);

public:
    Node(int id, const std::string& config_file);
    ~Node();

    void load_file(const std::string& data_file);
    void broadcast_done();
    void wait_for_completion();
    void print_stats();
};
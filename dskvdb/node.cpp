#include "node.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <chrono>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


Node::Node(int id, const std::string& config_file) : node_id(id)
{
    load_config(config_file);
    total_nodes = cluster.size();
    server_thread = std::thread(&Node::run_server, this);
}

Node::~Node()
{
    running = false;
    if (server_thread.joinable())
        server_thread.join();
}

void Node::load_config(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open cluster config file: " << filename << "\n";
        exit(1);
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);
        std::string id, ip, port;
        if (std::getline(ss, id, ',') && std::getline(ss, ip, ',')
            && std::getline(ss, port, ','))
        {
            cluster.push_back({std::stoi(id), ip, std::stoi(port)});
        }
    }
}

bool Node::send_message(const std::string& ip, int port, const std::string& msg)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return false;

    sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        close(sock);
        return false;
    }
    send(sock, msg.c_str(), msg.length(), 0);
    close(sock);

    return true;
}

void Node::handle_client(int client_sock)
{
    char buffer[1024];
    std::string recv_data;
    ssize_t bytes_read;

    while ((bytes_read = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        recv_data += buffer;
    }
    close(client_sock);

    if (recv_data.empty())
        return;

    if (recv_data.rfind("DATA", 0) == 0)
    {
        std::stringstream ss(recv_data);
        std::string token, key, val;
        std::getline(ss, token, '|'); // "DATA"
        std::getline(ss, key, '|');
        std::getline(ss, val, '|');

        db.store(std::stoi(key), val);
        records_stored++;
        records_received++;
    } 
    else if (recv_data == "DONE")
    {
        done_signals_received++;
    }
}

void Node::run_server() {
    int port = cluster[node_id].port;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Failed to create socket.\n";
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed on port " << port << "\n";
        return;
    }

    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "Listen failed on port " << port << "\n";
        return;
    }

    while (running)
    {
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(server_fd, &rfds);
        
        int retval = select(server_fd + 1, &rfds, NULL, NULL, &tv);
        if (retval > 0)
        {
            sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
            if (client_sock >= 0)
            {
                std::thread(&Node::handle_client, this, client_sock).detach();
            }
        }
    }

    close(server_fd);
}

void Node::load_file(const std::string& data_file)
{
    std::ifstream file(data_file);
    if (!file.is_open())
    {
        std::cerr << "Input file " << data_file << " not found.";
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string key_str, val;
        if (std::getline(ss, key_str, ',') && std::getline(ss, val, ','))
        {
            records_read++;
            int key = std::stoi(key_str);
            int owner = key % total_nodes;  // this can be hash function in common file

            if (owner == node_id)
            {
                db.store(key, val);
                records_stored++;
            }
            else
            {
                std::string msg = "DATA|" + std::to_string(key) + "|" + val;
                while (!send_message(cluster[owner].ip, cluster[owner].port, msg))
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
                records_sent++;
            }
        }
    }
}

void Node::broadcast_done()
{
    for (const auto& target_node : cluster)
    {
        if (target_node.id != node_id)
        {
            while (!send_message(target_node.ip, target_node.port, "DONE"))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
    }
}

void Node::wait_for_completion()
{
    int expected_signals = total_nodes - 1;
    while (done_signals_received < expected_signals)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Node::print_stats() {
    std::cout << "\n=========================================\n";
    std::cout << "          NODE " << node_id << " RUN STATISTICS          \n";
    std::cout << "=========================================\n";
    std::cout << "  Records Read from input file : " << records_read << "\n";
    std::cout << "  Records Sent (Remote)  : " << records_sent << "\n";
    std::cout << "  Records Recv (Remote)  : " << records_received << "\n";
    std::cout << "  Records Stored Locally : " << records_stored << "\n";
    std::cout << "  Database Size          : " << db.size() << "\n";
    std::cout << "-----------------------------------------\n";
    std::cout << "  Local Database Storage Contents:\n";
    db.print_contents();
    std::cout << "=========================================\n\n";
}
/**
Datastore class will manage the in memory database and provided the methods
for database operation handling the race conditions using mutex
*/

#pragma once
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <string>

class Datastore
{
    std::unordered_map<int, std::string> storage;
    std::mutex db_mutex;

public:
    Datastore() = default;

    void store(int key, const std::string& value)
    {
        std::lock_guard<std::mutex> lock(db_mutex);
        storage[key] = value;
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lock(db_mutex);
        return storage.size();
    }

    void print_contents()
    {
        std::lock_guard<std::mutex> lock(db_mutex);
        for (const auto& [key, val] : storage)
            std::cout << "  " << key << " -> " << val << "\n";
    }
};
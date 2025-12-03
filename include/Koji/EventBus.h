#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <cstdint>
#include <memory>
#include <atomic>
#include <algorithm>
#include <iostream>

class EventBus {
public:
    using HandlerFunc = std::function<void(const void*)>;

    struct Subscription {
        std::type_index type{typeid(void)};
        uint64_t id = 0;
    };

    EventBus() : nextId(1) {}

    // Subscribe to events of type Event
    template<typename Event>
    Subscription subscribe(std::function<void(const Event&)> handler) {
        auto wrapped = [handler](const void* ePtr) {
            handler(*static_cast<const Event*>(ePtr));
        };

        std::type_index ti{typeid(Event)};
        uint64_t id = nextId.fetch_add(1);
        listeners[ti].emplace_back(id, std::move(wrapped));
        return Subscription{ti, id};
    }

    void unsubscribe(const Subscription& sub) {
        auto it = listeners.find(sub.type);
        if (it == listeners.end()) return;
        auto &vec = it->second;
        vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const ListenerEntry &le) {
            return le.first == sub.id;
        }), vec.end());
        if (vec.empty()) listeners.erase(it);
    }

    // Emit event immediately
    template<typename Event>
    void emit(const Event& e) const {
        std::type_index ti{typeid(Event)};
        auto it = listeners.find(ti);
        if (it == listeners.end()) return;
        for (const auto &entry : it->second) {
            entry.second(&e);
        }
    }

    // Emit event to queue for later processing
    template<typename Event>
    void emitQueued(Event e) {
        queued.emplace_back([this, ev = std::move(e)]() mutable {
            std::type_index ti{typeid(Event)};
            auto it = listeners.find(ti);
            if (it == listeners.end()) return;
            for (const auto &entry : it->second) {
                entry.second(&ev);
            }
        });
    }

    void processQueued() {
        for (auto &f : queued) f();
        queued.clear();
    }

    void clearAll() {
        listeners.clear();
        queued.clear();
    }

private:
    using ListenerEntry = std::pair<uint64_t, HandlerFunc>;
    std::unordered_map<std::type_index, std::vector<ListenerEntry>> listeners;
    std::vector<std::function<void()>> queued;
    std::atomic<uint64_t> nextId;
};

#include <engine/render/render_graph.hpp>
#include <engine/window/window_system.hpp>

#include <algorithm>
#include <iostream>
#include <queue>
#include <stdexcept>

namespace engine {

void RenderGraph::compile() {
    topological_sort();
    _compiled = true;
}

void RenderGraph::execute() {
    if (!_compiled) {
        compile();
    }

    for (size_t idx : _execution_order) {
        _passes[idx]->execute();
    }

    // Swap buffers
    window::WindowSystem::get_instance().swap_buffers();
}

void RenderGraph::topological_sort() {
    size_t n = _passes.size();
    if (n == 0) return;

    // Build resource -> producer pass mapping
    std::unordered_map<std::string, size_t> resource_producer;
    for (size_t i = 0; i < n; ++i) {
        for (const auto& resource : _passes[i]->writes()) {
            resource_producer[resource] = i;
        }
    }

    // Build adjacency list (dependencies)
    std::vector<std::vector<size_t>> adj(n);
    std::vector<size_t> in_degree(n, 0);

    for (size_t i = 0; i < n; ++i) {
        for (const auto& resource : _passes[i]->reads()) {
            auto it = resource_producer.find(resource);
            if (it != resource_producer.end() && it->second != i) {
                // Pass i depends on pass it->second
                adj[it->second].push_back(i);
                in_degree[i]++;
            }
        }
    }

    // Kahn's algorithm for topological sort
    std::queue<size_t> queue;
    for (size_t i = 0; i < n; ++i) {
        if (in_degree[i] == 0) {
            queue.push(i);
        }
    }

    _execution_order.clear();
    _execution_order.reserve(n);

    while (!queue.empty()) {
        size_t curr = queue.front();
        queue.pop();
        _execution_order.push_back(curr);

        for (size_t next : adj[curr]) {
            in_degree[next]--;
            if (in_degree[next] == 0) {
                queue.push(next);
            }
        }
    }

    if (_execution_order.size() != n) {
        std::cerr << "ERROR::RENDER_GRAPH::Cycle detected in render graph!" << std::endl;
        // Fall back to original order
        _execution_order.clear();
        for (size_t i = 0; i < n; ++i) {
            _execution_order.push_back(i);
        }
    }
}

}  // namespace engine

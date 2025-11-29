#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace engine {

/// Base class for all render passes
class RenderPass {
public:
    explicit RenderPass(std::string name) : _name(std::move(name)) {}
    virtual ~RenderPass() = default;

    const std::string& name() const { return _name; }

    /// Declare resources this pass reads from
    const std::unordered_set<std::string>& reads() const { return _reads; }

    /// Declare resources this pass writes to
    const std::unordered_set<std::string>& writes() const { return _writes; }

    /// Execute the pass
    virtual void execute() = 0;

protected:
    void reads_resource(const std::string& resource) { _reads.insert(resource); }
    void writes_resource(const std::string& resource) { _writes.insert(resource); }

private:
    std::string _name;
    std::unordered_set<std::string> _reads;
    std::unordered_set<std::string> _writes;
};

/// Render graph that manages pass dependencies and execution order
class RenderGraph {
public:
    RenderGraph() = default;

    /// Add a pass to the graph and return a raw pointer to it
    template<typename T>
    T* add_pass(std::unique_ptr<T> pass) {
        T* ptr = pass.get();
        _passes.push_back(std::move(pass));
        _compiled = false;
        return ptr;
    }

    /// Compile the graph (resolve dependencies, determine execution order)
    /// Call this after all passes are added
    void compile();

    /// Execute all passes in dependency order and swap buffers
    void execute();

    /// Check if graph has been compiled
    bool is_compiled() const { return _compiled; }

private:
    std::vector<std::unique_ptr<RenderPass>> _passes;
    std::vector<size_t> _execution_order;
    bool _compiled = false;

    void topological_sort();
};

}  // namespace engine

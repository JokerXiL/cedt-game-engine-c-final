#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace engine::resource {

/// Generic resource cache template
/// @tparam Key      The key type used for lookups (typically std::string)
/// @tparam Resource The resource type being cached (e.g., Shader, Texture)
/// @tparam Loader   A callable type that creates resources from keys
template <typename Key, typename Resource, typename Loader>
class Cache {
public:
    using ResourcePtr = std::shared_ptr<Resource>;
    using MapType = std::unordered_map<Key, ResourcePtr>;

    /// Construct with a loader function/functor
    explicit Cache(Loader loader) : _loader(std::move(loader)) {}

    /// Load resource by key, returns cached version if already loaded
    template <typename... Args>
    ResourcePtr load(Args&&... args) {
        Key key = _loader.make_key(std::forward<Args>(args)...);

        auto it = _cache.find(key);
        if (it != _cache.end()) {
            return it->second;
        }

        ResourcePtr resource = _loader(std::forward<Args>(args)...);
        if (resource) {
            _cache[key] = resource;
        }
        return resource;
    }

    /// Check if resource is cached
    template <typename... Args>
    bool contains(Args&&... args) const {
        return _cache.find(_loader.make_key(std::forward<Args>(args)...)) != _cache.end();
    }

    /// Remove all cached resources
    void clear() { _cache.clear(); }

    /// Number of cached resources
    size_t size() const { return _cache.size(); }

    /// Get read-only access to the cache (for iteration/debugging)
    const MapType& entries() const { return _cache; }

private:
    Loader _loader;
    MapType _cache;
};

}  // namespace engine::resource

#pragma once

#include <string>
#include <map>
#include "common/slot_map.h"
#include <fstream>
#include <filesystem>

namespace sprint::resources {

namespace details {

static bool ReadFile(const std::filesystem::path& path, std::ostream& stream) {
    std::string content;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path, std::ios::in | std::ios::binary);
        stream << file.rdbuf();
        file.close();
        return true;
    }
    catch (std::ifstream::failure& e) {
        log::core::Error("Couldn't read file {0}", path.string());
    }
    return false;
}

template<class T>
class ResourceStore {
public:
    using ResourceTable = stdext::slot_map<std::pair<std::unique_ptr<T>, uint32_t>>;
    using ResourceID = typename ResourceTable::key_type;

public:
    ResourceStore() = default;
    ~ResourceStore() = default;

    ResourceID GetOrLoad(const std::string& path) {
        if (auto it = path_to_keys_.find(path); it != path_to_keys_.end()) {
            return it->second;
        }

        std::stringstream file_stream;
        std::stringstream meta_stream;

        ReadFile(path, file_stream);
        ReadFile(path + ".meta", meta_stream);

        ResourceID id = table_.insert({ T::Load(file_stream, meta_stream), 0 });
        path_to_keys_.insert({path, id});
        return id;
    }

    T* Get(ResourceID id) { return table_[id].first.get(); }
    const T* Get(ResourceID id) const { return table_[id].first.get(); }

    void IncRefCount(ResourceID id) {
        table_[id].second++;
    }

    void DecRefCount(ResourceID id) {
        assert(table_[id].second > 0);

        uint32_t count = --table_[id].second;
        if (count == 0) {
            table_.erase(id);

            auto it = std::find_if(path_to_keys_.begin(), path_to_keys_.end(), [&](const auto& pair){
                return pair.second == id;
            });
            if (it != path_to_keys_.end()) {
                path_to_keys_.erase(it);
            }
        }
    }

private:
    ResourceTable table_ = {};
    std::unordered_map<std::string, ResourceID> path_to_keys_ = {};
};

template<class T>
ResourceStore<T>& GetStore() {
    static ResourceStore<T> instance;
    return instance;
}

}

template<class T>
class ResourceHandle {
public:
    using ResourceID = typename details::ResourceStore<T>::ResourceID;

public:
    ResourceHandle() noexcept = default;

    explicit ResourceHandle(details::ResourceStore<T>& store, ResourceID id)
        : store_(&store)
        , id_(id) {
        IncRefCount();
    }

    ~ResourceHandle() {
        Reset();
    }

    ResourceHandle(const ResourceHandle& other)
        : store_(other.store_)
        , id_(other.id_) {
        IncRefCount();
    }

    ResourceHandle(ResourceHandle&& other) noexcept
        : store_(other.store_)
        , id_(other.id_) {
        other.Release();
    }

    ResourceHandle& operator=(const ResourceHandle& other) {
        ResourceHandle(other).Swap(*this);
        return *this;
    }

    ResourceHandle& operator=(ResourceHandle&& other) noexcept {
        ResourceHandle(std::move(other)).Swap(*this);
        return *this;
    }

    explicit operator bool() const noexcept { return store_ != nullptr; }

    T* operator->() { return Get(); }
    T& operator*() { return *Get(); }

    const T* operator->() const { return Get(); }
    const T& operator*() const { return *Get(); }

    T* Get() { return store_->Get(id_); }
    const T* Get() const { return store_->Get(id_); }

    void Reset() {
        DecRefCount();
        Release();
    }

private:
    void Release() {
        store_ = nullptr;
    }

    void IncRefCount() {
        if (store_)
            store_->IncRefCount(id_);
    }

    void DecRefCount() {
        if (store_)
            store_->DecRefCount(id_);
    }

    void Swap(ResourceHandle& other) {
        std::swap(store_, other.store_);
        std::swap(id_, other.id_);
    }

private:
    details::ResourceStore<T>* store_ = nullptr;
    ResourceID id_ = {};
};

template<class T>
ResourceHandle<T> Load(const std::string& path) {
    details::ResourceStore<T>& store = details::GetStore<T>();
    return ResourceHandle<T>(store, store.GetOrLoad(path));
}

}

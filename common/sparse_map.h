#pragma once

#include "sparse_set.h"

template<class KeyType, class ValueType>
class sparse_map : public sparse_set<KeyType> {
private:
    using iterator       = typename std::vector<ValueType>::iterator;
    using const_iterator = typename std::vector<ValueType>::const_iterator;

public:
    using set = sparse_set<KeyType>;

public:
    auto begin() { return values_.begin(); }
    auto end() { return values_.end(); }

    auto begin() const { return values_.begin(); }
    auto end() const { return values_.end(); }

    auto rbegin() { return values_.rbegin(); }
    auto rend() { return values_.rend(); }

    auto rbegin() const { return values_.rbegin(); }
    auto rend() const { return values_.rend(); }

    ValueType& push(KeyType key, const ValueType& value) {
        assert(!set::contains(key));
        values_.push_back(value);
        set::insert(key);
        return values_.back();
    }

    ValueType& push(KeyType key, ValueType&& value) {
        assert(!set::contains(key));
        values_.push_back(std::move(value));
        set::insert(key);
        return values_.back();
    }

    template<class ...Args>
    ValueType& emplace(KeyType key, Args&&... args) {
        assert(!set::contains(key));
        values_.emplace_back(std::forward<Args>(args)...);
        set::insert(key);
        return values_.back();
    }

    void erase(KeyType key) {
        assert(set::contains(key));
        values_[set::index(key)] = std::move(values_.back());
        values_.pop_back();
        set::erase(key);
    }

    [[nodiscard]] ValueType& get(KeyType key) { return values_[set::index(key)]; }
    [[nodiscard]] const ValueType& get(KeyType key) const { return values_[set::index(key)]; }

    [[nodiscard]] ValueType* try_get(KeyType key) { return set::contains(key) ? &get(key) : nullptr; }
    [[nodiscard]] const ValueType* try_get(KeyType key) const  { return set::contains(key) ? &get(key) : nullptr; }

private:
    std::vector<ValueType> values_;
};
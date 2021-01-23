#pragma once

#include <cstdint>
#include <Log.h>
#include "sparse_set.h"

namespace sprint::ecs {

using entity_t = uint32_t;
using component_id = uint32_t;

namespace details {

struct entity_traits {
    static constexpr uint8_t  gen_offset = 20u;
    static constexpr entity_t index_mask       = 0x000FFFFF;
    static constexpr entity_t generation_mask  = 0xFFFu << gen_offset;

    static constexpr entity_t get_index(entity_t entity) { return entity & index_mask; }
    static constexpr entity_t get_generation(entity_t entity) { return (entity & generation_mask) >> gen_offset; }

    static constexpr void set_index(entity_t& entity, entity_t idx) { entity = (entity & generation_mask) | idx; }
    static constexpr void set_generation(entity_t& entity, entity_t gen) { entity = (entity & index_mask) | (gen << gen_offset); }
};

template<class Component>
class component_pool : public sparse_set<entity_t> {
private:
    using iterator       = typename std::vector<Component>::iterator;
    using const_iterator = typename std::vector<Component>::const_iterator;

public:
    using entities = sparse_set<entity_t>;

public:
    auto begin() { return components_.begin(); }
    auto end() { return components_.end(); }

    auto begin() const { return components_.begin(); }
    auto end() const { return components_.end(); }

    auto rbegin() { return components_.rbegin(); }
    auto rend() { return components_.rend(); }

    auto rbegin() const { return components_.rbegin(); }
    auto rend() const { return components_.rend(); }

    Component& push(entity_t entity, const Component& component) {
        assert(!entities::contains(entity));
        components_.push_back(component);
        entities::insert(entity);
        return components_.back();
    }

    Component& push(entity_t entity, Component&& component) {
        assert(!entities::contains(entity));
        components_.push_back(std::move(component));
        entities::insert(entity);
        return components_.back();
    }

    template<class ...Args>
    Component& emplace(entity_t entity, Args&&... args) {
        assert(!entities::contains(entity));
        components_.emplace_back(std::forward<Args>(args)...);
        entities::insert(entity);
        return components_.back();
    }

    void erase(entity_t entity) {
        assert(entities::contains(entity));
        components_[entities::index(entity)] = std::move(components_.back());
        components_.pop_back();
        entities::erase(entity);
    }

    [[nodiscard]] Component& get(entity_t entity) { return components_[entities::index(entity)]; }
    [[nodiscard]] const Component& get(entity_t entity) const { return components_[entities::index(entity)]; }

    [[nodiscard]] Component* try_get(entity_t entity) { return entities::contains(entity) ? &get(entity) : nullptr; }
    [[nodiscard]] const Component* try_get(entity_t entity) const  { return entities::contains(entity) ? &get(entity) : nullptr; }

private:
    std::vector<Component> components_;
};

component_id next_id();

}

template<class Component>
struct component_info {
public:
    static component_id id() {
        static component_id id = details::next_id();
        return id;
    }
};

template<class ...>
class component_view;

template<class Component>
class component_view<Component> {
private:
    using pool_t = details::component_pool<Component>;
    using entities_t = sparse_set<entity_t>;
public:
    explicit component_view(pool_t& pool) : pool_(&pool) {}

    auto begin() const { return pool_->entities_t::begin(); }
    auto end() const { return pool_->entities_t::end(); }

    [[nodiscard]] Component &get(entity_t entity) { return pool_->get(entity); }
    [[nodiscard]] const Component &get(entity_t entity) const { return pool_->get(entity); }

    [[nodiscard]] Component* try_get(entity_t entity) { return pool_->contains(entity) ? &get(entity) : nullptr; }
    [[nodiscard]] const Component* try_get(entity_t entity) const { return pool_->contains(entity) ? &get(entity) : nullptr; }

    [[nodiscard]] bool contains(entity_t entity) const { return pool_->contains(entity); }

private:
    pool_t* pool_;
};

template<class ...Components>
class component_view {
private:
    static_assert(sizeof...(Components) > 1, "Invalid components");

    template<class Comp>
    using pool_t = details::component_pool<Comp>;
    using entities_t = sparse_set<entity_t>;
    using unchecked_array = std::array<const entities_t*, sizeof...(Components) - 1>;

private:
    template<class It>
    struct view_iterator {
    public:
        using difference_type = typename std::iterator_traits<It>::difference_type;
        using value_type = typename std::iterator_traits<It>::value_type;
        using pointer = typename std::iterator_traits<It>::pointer;
        using reference = typename std::iterator_traits<It>::reference;
        using iterator_category = std::bidirectional_iterator_tag;

    private:
        [[nodiscard]] bool check() const {
            return std::all_of(unchecked_.begin(), unchecked_.end(), [entity = *curr_](const entities_t* pool) { return pool->contains(entity); });
        }

    public:
        view_iterator(It first, It last, It curr, unchecked_array unchecked)
            : first_(first), last_(last), curr_(curr), unchecked_(unchecked) {
            if (curr_ != last_ && !check()) ++(*this);
        }

        view_iterator& operator++() {
            while (++curr_ != last_ && !check());
            return *this;
        };

        view_iterator& operator--() {
            while (--curr_ != first_ && !check());
            return *this;
        };

        view_iterator operator++(int) {
            view_iterator orig = *this;
            return ++(*this), orig;
        };

        view_iterator operator--(int) {
            view_iterator orig = *this;
            return --(*this), orig;
        };

        [[nodiscard]] bool operator==(const view_iterator& other) const {
            return other.curr_ == curr_;
        }

        [[nodiscard]] bool operator!=(const view_iterator& other) const {
            return !(*this == other);
        }

        [[nodiscard]] pointer operator->() const {
            return curr_.operator->();
        }

        [[nodiscard]] reference operator*() const {
            return curr_.operator*();
        }

    private:
        It first_, last_, curr_;
        unchecked_array unchecked_;
    };

    static const entities_t* shortest(const pool_t<Components>&... pools) {
        return std::min({static_cast<const entities_t*>(&pools)...}, [](const entities_t* lhs, const entities_t* rhs) {
            return lhs->size() < rhs->size();
        });
    }

    static unchecked_array except(const entities_t* exception, const pool_t<Components>*... pools) {
        unchecked_array arr;
        size_t idx = 0;
        ((static_cast<const entities_t*>(pools) != exception ? arr[idx++] = pools : nullptr), ...);
        return arr;
    }

    unchecked_array unchecked() const { return except(entities_, std::get<pool_t<Components>*>(pools_)...); }

public:
    using iterator = view_iterator<typename entities_t::const_iterator>;

public:
    explicit component_view(pool_t<Components>&... pools)
        : pools_{&pools...}, entities_{shortest(pools...)} {}

    [[nodiscard]] bool contains(entity_t entity) const {
        return (std::get<pool_t<Components>*>(pools_)->contains(entity) && ...);
    }

    iterator begin() const {
        return iterator { entities_->begin(), entities_->end(), entities_->begin(), unchecked() };
    }

    iterator end() const {
        return iterator { entities_->begin(), entities_->end(), entities_->end(), unchecked() };
    }

    template<class ...Comp>
    decltype(auto) get(entity_t entity) const {
        assert(contains(entity));

        if constexpr (sizeof...(Comp) == 0) {
            return get<Components...>(entity);
        }
        else if constexpr (sizeof...(Comp) == 1) {
            return (std::get<pool_t<Comp> *>(pools_)->get(entity), ...);
        }
        else {
            return std::tuple<Comp&...>{get<Comp>(entity)...};
        }
    }

private:
    const std::tuple<pool_t<Components>*...> pools_;
    const sparse_set<entity_t>* entities_;
};



struct entity_set {
private:
    static constexpr size_t invalid_idx = details::entity_traits::index_mask;

    template<class Component>
    using pool_t = details::component_pool<Component>;
    using pool_ptr = std::unique_ptr<sparse_set<entity_t>>;
    using entity_traits = details::entity_traits;

    template<class Component>
    pool_t<Component>& assure() {
        component_id id = component_info<Component>::id();
        if (id >= pools_.size()) {
            pools_.resize(id+1);
        }

        if (!pools_[id]) {
            pools_[id].reset(new pool_t<Component>);
        }

        return *static_cast<pool_t<Component>*>(pools_[id].get());
    }

    template<class Component>
    const pool_t<Component>* get_pool() const {
        component_id id = component_info<Component>::id();
        assert(id < pools_.size());
        return pools_[id] ? static_cast<pool_t<Component>*>(pools_[id].get()) : nullptr;
    }

    template<class Component>
    pool_t<Component>* get_pool() {
        component_id id = component_info<Component>::id();
        assert(id < pools_.size());
        return pools_[id] ? static_cast<pool_t<Component>*>(pools_[id].get()) : nullptr;
    }

public:
    entity_t create() {
        entity_t entity{};
        if (free_idx_ == invalid_idx) {
            entity_traits::set_index(entity, entities_.size());
            entity_traits::set_generation(entity, {});
            entities_.push_back(entity);
        } else {
            entity_t free = entities_[free_idx_];

            entity_traits::set_index(entity, free_idx_);
            entity_traits::set_generation(entity, entity_traits::get_generation(free));
            entities_[free_idx_] = entity;

            free_idx_ = entity_traits::get_index(free);
        }
        assert(valid(entity));
        return entity;
    }

    void destroy(entity_t entity) {
        assert(valid(entity));
        auto index = details::entity_traits::get_index(entity);
        details::entity_traits::set_index(entities_[index], free_idx_);
        details::entity_traits::set_generation(entities_[index], entity_traits::get_generation(entities_[index]) + 1);
        free_idx_ = details::entity_traits::get_index(entity);
    }

    bool valid(entity_t entity) const {
        return entities_[entity_traits::get_index(entity)] == entity;
    }

    template<class Component, class ...Args>
    Component& emplace(entity_t entity, Args &&... args) {
        assert(valid(entity));
        return assure<Component>().emplace(entity, std::forward<Args>(args)...);
    }

    template<class Component>
    void remove(entity_t entity) {
        assert(has<Component>(entity));
        get_pool<Component>()->erase(entity);
    }

    template<class Component>
    Component& get(entity_t entity) {
        assert(has<Component>(entity));
        return get_pool<Component>()->get(entity);
    }

    template<class Component>
    const Component& get(entity_t entity) const {
        assert(has<Component>(entity));
        return get_pool<Component>()->get(entity);
    }

    template<class Component>
    Component* try_get(entity_t entity) {
        assert(valid(entity));
        if (auto pool = get_pool<Component>(); pool) {
            return pool->try_get(entity);
        }
        return nullptr;
    }

    template<class Component>
    const Component* try_get(entity_t entity) const {
        assert(valid(entity));
        if (auto pool = get_pool<Component>(); pool) {
            return pool->try_get(entity);
        }
        return nullptr;
    }

    template<class Component>
    [[nodiscard]] bool has(entity_t entity) const {
        assert(valid(entity));
        auto id = component_info<Component>::id();
        return id < pools_.size() && pools_[id] && pools_[id]->contains(entity);
    }

    template<class ...Component>
    component_view<Component...> view() {
        return component_view<Component...>(assure<Component>()...);
    }

private:
    std::vector<pool_ptr> pools_{};
    std::vector<entity_t> entities_{};
    size_t free_idx_{invalid_idx};
};

}

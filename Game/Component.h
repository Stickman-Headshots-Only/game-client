#pragma once

#include <memory>
#include <type_traits>

class Component;
using ComponentPtr = std::unique_ptr<Component>;

class Component
{
public:
    template <typename CompT>
    static std::size_t GetID()
    {
        static_assert(std::is_base_of<Component, CompT>, "Error: CompT is not derived from Component");
        static_assert(!std::is_same_v<Component, CompT>, "Error: CompT is same as Component");

        static const std::size_t id = sMaxID++;
        return id;
    }

    virtual ~Component() = default;

protected:
    Component() = default;
    Component(const Component&) = default;
    Component(Component&&) noexcept = default;

    Component& operator=(const Component&) = default;
    Component& operator=(Component&&) noexcept = default;
private:
    static inline std::size_t sMaxID = 0;

};


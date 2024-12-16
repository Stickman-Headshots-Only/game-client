#pragma once

#include "Component.h"
#include "Bitset.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace Rei
{

class Entity;
using EntityPtr = std::unique_ptr<Entity>;

class Entity
{
public:
    explicit Entity(std::size_t index, bool enabled = true) : m_id{ index }, m_enabled{ enabled } {}
    Entity(const Entity&) = delete;
    Entity(Entity&&) noexcept = delete;

    std::size_t getId() const noexcept { return m_id; }
    bool isEnabled() const noexcept { return m_enabled; }
    const std::vector<ComponentPtr>& getComponents() const noexcept { return m_components; }
    const Bitset& getEnabledComponents() const noexcept { return m_enabledComponents; }

    template <typename... Args>
    static EntityPtr create(Args&&... args)
    {
        return std::make_unique<Entity>(std::forward<Args>(args)...);
    }

    void enable(bool enabled = true) noexcept { m_enabled = enabled; }
    void disable() noexcept { enable(false); }

    template <typename CompT, typename... Args>
    CompT& addComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, CompT>, "Error: The added component must be derived from Component.");

        const std::size_t compId = Component::getId<CompT>();

        if (compId >= m_components.size())
            m_components.resize(compId + 1);

        m_components[compId] = std::make_unique<CompT>(std::forward<Args>(args)...);
        m_enabledComponents.setBit(compId);

        return static_cast<CompT&>(*m_components[compId]);
    }

    template <typename CompT>
    std::tuple<CompT&> addComponents()
    {
        static_assert(std::is_base_of_v<Component, CompT>, "Error: The added component must be derived from Component.");

        return std::forward_as_tuple(addComponent<CompT>());
    }

    template <typename Comp1, typename Comp2, typename... C>
    std::tuple<Comp1&, Comp2&, C...> addComponents()
    {
        static_assert(std::is_base_of_v<Component, CompT1>, "Error: The added component must be derived from Component.");

        return std::tuple_cat(std::forward_as_tuple(addComponent<CompT1>()), addComponents<CompT2, C...>());
    }

    template <typename CompT>
    bool hasComponent() const
    {
        static_assert(std::is_base_of_v<Component, CompT>, "Error: The checked component must be derived from Component.");

        const std::size_t compId = Component::getId<CompT>();
        return ((compId < m_components.size()) && m_enabledComponents[compId]);
    }

    template <typename CompT>
    const CompT& getComponent() const
    {
        static_assert(std::is_base_of_v<Component, CompT>, "Error: The fetched component must be derived from Component.");

        if (hasComponent<CompT>())
            return static_cast<const CompT&>(*m_components[Component::getId<CompT>()]);

        throw std::runtime_error("Error: No component available of specified type");
    }

    template <typename CompT>
    CompT& getComponent()
    {
        return const_cast<CompT&>(static_cast<const Entity*>(this)->getComponent<CompT>());
    }

    template <typename CompT> void removeComponent()
    {
        static_assert(std::is_base_of_v<Component, CompT>, "Error: The removed component must be derived from Component.");

        if (hasComponent<CompT>())
        {
            const std::size_t compId = Component::getId<CompT>();

            m_components[compId].reset();
            m_enabledComponents.setBit(compId, false);
        }
    }

    Entity& operator=(const Entity&) = delete;
    Entity& operator=(Entity&&) noexcept = delete;

private:
    std::size_t m_id{};
    bool m_enabled{};
    std::vector<ComponentPtr> m_components{};
    Bitset m_enabledComponents{};
};

} // namespace Rei

#pragma once

#include <vector>

#include "Entity.h"
#include "Bitset.h"

namespace Rei
{


    struct FrameTimeInfo;
    class System;
    using SystemPtr = std::unique_ptr<System>;

    class System
    {
        friend class World;
    public:
        System(const System&) = delete;
        System(System&&) noexcept = delete;

        const Bitset& getAcceptedComponents() const noexcept
        {
            return m_acceptedComponents;
        }

        template <typename SysT>
        static std::size_t getId()
        {
            static_assert(std::is_base_of_v<System, SysT>, "Error: The fetched system must be derived from System.");
            static_assert(!std::is_same_v<System, SysT>, "Error: The fetched system must not be of specific type 'System'.");

            static const std::size_t id = s_maxId++;
            return id;
        }

        bool containsEntity(const Entity& entity) const noexcept
        {
            for (const Entity* entityPtr : m_entities)
            {
                if (entityPtr->getId() == entity.getId())
                {
                    return true;
                }
            }

            return false;
        }

        virtual bool update([[maybe_unused]] const FrameTimeInfo& timeInfo) { return true; }

        virtual void destroy() {}

        System& operator=(const System&) = delete;
        System& operator=(System&&) noexcept = delete;

        virtual ~System() = default;
    protected:
        System() = default;

        template <typename... CompTs>
        void registerComponents()
        {
            (m_acceptedComponents.setBit(Component::getId<CompTs>()), ...);
        }

        template <typename... CompTs>
        void unregisterComponents()
        {
            (m_acceptedComponents.setBit(Component::getId<CompTs>(), false), ...);
        }

        virtual void linkEntity(const EntityPtr& entity)
        {
            m_entities.emplace_back(entity.get());
        }

        virtual void unlinkEntity(const EntityPtr& entity)
        {
            for (std::size_t entityIndex = 0; entityIndex < m_entities.size(); ++entityIndex)
            {
                if (m_entities[entityIndex]->getId() == entity->getId())
                {
                    m_entities.erase(m_entities.begin() + static_cast<std::ptrdiff_t>(entityIndex));
                    break;
                }
            }
        }

        std::vector<Entity*> m_entities{};
        Bitset m_acceptedComponents{};

    private:
        static inline std::size_t s_maxId = 0;
    };

} // namespace Rei
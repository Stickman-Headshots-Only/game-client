#pragma once

#include <stdexcept>

#include "Entity.h"
#include "System.h"

namespace Rei
{


    struct FrameTimeInfo;
    class World;
    using WorldPtr = std::unique_ptr<World>;

    class World
    {
    public:
        World() = default;
        explicit World(std::size_t entityCount) { m_entities.reserve(entityCount); }
        World(const World&) = delete;
        World(World&&) noexcept = default;

        const std::vector<SystemPtr>& getSystems() const { return m_systems; }
        const std::vector<EntityPtr>& getEntities() const { return m_entities; }

  
        template <typename SysT, typename... Args>
        SysT& addSystem(Args&&... args)
        {
            static_assert(std::is_base_of_v<System, SysT>, "Error: The added system must be derived from System.");

            const std::size_t systemId = System::getId<SysT>();

            if (systemId >= m_systems.size())
                m_systems.resize(systemId + 1);

            m_systems[systemId] = std::make_unique<SysT>(std::forward<Args>(args)...);
            m_activeSystems.setBit(systemId);

            return static_cast<SysT&>(*m_systems[systemId]);
        }

        template <typename SysT>
        bool hasSystem() const
        {
            static_assert(std::is_base_of_v<System, SysT>, "Error: The checked system must be derived from System.");

            const std::size_t systemId = System::getId<SysT>();
            return ((systemId < m_systems.size()) && m_systems[systemId]);
        }

        template <typename SysT>
        const SysT& getSystem() const
        {
            static_assert(std::is_base_of_v<System, SysT>, "Error: The fetched system must be derived from System.");

            if (hasSystem<SysT>())
                return static_cast<const SysT&>(*m_systems[System::getId<SysT>()]);

            throw std::runtime_error("Error: No system available of specified type");
        }

        template <typename SysT>
        SysT& getSystem()
        {
            return const_cast<SysT&>(static_cast<const World*>(this)->getSystem<SysT>());
        }

        template <typename SysT>
        void removeSystem()
        {
            static_assert(std::is_base_of_v<System, SysT>, "Error: The removed system must be derived from System.");

            if (hasSystem<SysT>())
                m_systems[System::getId<SysT>()].reset();
        }

        Entity& addEntity(bool enabled = true)
        {
            m_entities.emplace_back(Entity::create(m_maxEntityIndex++, enabled));
            m_activeEntityCount += enabled;

            return *m_entities.back();
        }

        template <typename CompT, typename... Args>
        Entity& addEntityWithComponent(Args&&... args)
        {
            Entity& entity = addEntity();
            entity.addComponent<CompT>(std::forward<Args>(args)...);

            return entity;
        }

        template <typename... CompsTs>
        Entity& addEntityWithComponents(bool enabled = true)
        {
            Entity& entity = addEntity(enabled);
            entity.addComponents<CompsTs...>();

            return entity;
        }

        template <typename... CompsTs>
        std::vector<Entity*> recoverEntitiesWithComponents()
        {
            static_assert((std::is_base_of_v<Component, CompsTs> && ...), "Error: The components to query the entity with must all be derived from Component.");

            std::vector<Entity*> entities;

            for (const EntityPtr& entity : m_entities)
            {
                if ((entity->hasComponent<CompsTs>() && ...))
                    entities.emplace_back(entity.get());
            }

            return entities;
        }

        void removeEntity(const Entity& entity)
        {
            auto iter = std::find_if(m_entities.begin(), m_entities.end(), [&entity](const EntityPtr& entityPtr) { return (&entity == entityPtr.get()); });

            if (iter == m_entities.end())
                throw std::invalid_argument("Error: The entity isn't owned by this world");

            for (const SystemPtr& system : m_systems)
                system->unlinkEntity(*iter);

            m_entities.erase(iter);
        }

        bool update(const FrameTimeInfo& timeInfo)
        {
            // ZoneScopedN("World::update");

            refresh();

            for (std::size_t systemIndex = 0; systemIndex < m_systems.size(); ++systemIndex)
            {
                if (!m_activeSystems[systemIndex])
                    continue;

                const bool isSystemActive = m_systems[systemIndex]->update(timeInfo);

                if (!isSystemActive)
                    m_activeSystems.setBit(systemIndex, false);
            }

            return !m_activeSystems.isEmpty();
        }

        void refresh()
        {
            // ZoneScopedN("World::refresh");

            if (m_entities.empty())
                return;

            sortEntities();

            for (std::size_t entityIndex = 0; entityIndex < m_activeEntityCount; ++entityIndex)
            {
                const EntityPtr& entity = m_entities[entityIndex];

                if (!entity->isEnabled())
                    continue;

                for (std::size_t systemIndex = 0; systemIndex < m_systems.size(); ++systemIndex)
                {
                    const SystemPtr& system = m_systems[systemIndex];

                    if (system == nullptr || !m_activeSystems[systemIndex])
                        continue;

                    const Bitset matchingComponents = system->getAcceptedComponents() & entity->getEnabledComponents();

                    if (!system->containsEntity(*entity))
                    {
                        if (!matchingComponents.isEmpty())
                            system->linkEntity(entity);
                    }
                    else
                    {
                        if (matchingComponents.isEmpty())
                            system->unlinkEntity(entity);
                    }
                }
            }
        }

        void destroy()
        {
            // ZoneScopedN("World::destroy");

            m_entities.clear();
            m_activeEntityCount = 0;
            m_maxEntityIndex = 0;

            for (const SystemPtr& system : m_systems)
            {
                if (system)
                    system->m_entities.clear();
            }

            m_systems.clear();
            m_activeSystems.clear();
        }

        World& operator=(const World&) = delete;
        World& operator=(World&&) noexcept = default;

        ~World() { destroy(); }

    private:

        void sortEntities()
        {
            // ZoneScopedN("World::sortEntities");

            auto firstEntity = m_entities.begin();
            auto lastEntity = m_entities.end() - 1;

            while (firstEntity != lastEntity)
            {
                if ((*firstEntity)->isEnabled())
                {
                    ++firstEntity;
                    continue;
                }

                while (firstEntity != lastEntity && (*lastEntity == nullptr || !(*lastEntity)->isEnabled()))
                {
                    --lastEntity;
                }

                if (firstEntity == lastEntity)
                {
                    break;
                }

                std::swap(*firstEntity, *lastEntity);
                --lastEntity;
            }

            m_activeEntityCount = static_cast<std::size_t>(std::distance(m_entities.begin(), lastEntity) + 1);
        }

        std::vector<SystemPtr> m_systems{};
        Bitset m_activeSystems{};

        std::vector<EntityPtr> m_entities{};
        std::size_t m_activeEntityCount = 0;
        std::size_t m_maxEntityIndex = 0;
    };
}
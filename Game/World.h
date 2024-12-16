#pragma once


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

  
        template <typename SysT, typename... Args> SysT& addSystem(Args&&... args);
        template <typename SysT> bool hasSystem() const;
        template <typename SysT> const SysT& getSystem() const;
        template <typename SysT> SysT& getSystem() { return const_cast<SysT&>(static_cast<const World*>(this)->getSystem<SysT>()); }
        template <typename SysT> void removeSystem();
        Entity& addEntity(bool enabled = true);
        template <typename CompT, typename... Args> Entity& addEntityWithComponent(Args&&... args);
        template <typename... CompsTs> Entity& addEntityWithComponents(bool enabled = true);
        template <typename... CompsTs> std::vector<Entity*> recoverEntitiesWithComponents();
        void removeEntity(const Entity& entity);
        bool update(const FrameTimeInfo& timeInfo);
        void refresh();
        void destroy();

        World& operator=(const World&) = delete;
        World& operator=(World&&) noexcept = default;

        ~World() { destroy(); }

    private:

        void sortEntities();

        std::vector<SystemPtr> m_systems{};
        Bitset m_activeSystems{};

        std::vector<EntityPtr> m_entities{};
        std::size_t m_activeEntityCount = 0;
        std::size_t m_maxEntityIndex = 0;
    };
}
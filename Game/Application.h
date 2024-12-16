#pragma once

#include <cassert>
#include <chrono>
#include <memory>

#include "World.h"
#include "Bitset.h"
#include "Logger.h"


namespace Rei
{
    struct FrameTimeInfo
    {
        float deltaTime{};
        float globalTime{};
        int substepCount{};
        float substepTime{};
    };

    class Application
    {
    public:
        explicit Application(std::size_t worldCount = 1)
        {
            m_worlds.reserve(worldCount);
        }

        const std::vector<WorldPtr>& getWorlds() const { return m_worlds; }
        std::vector<WorldPtr>& getWorlds() { return m_worlds; }
        const FrameTimeInfo& getTimeInfo() const { return m_timeInfo; }

        void setFixedTimeStep(float fixedTimeStep) {
            assert("Error: Fixed time step must be positive." && fixedTimeStep > 0.f);
            m_timeInfo.substepTime = fixedTimeStep;
        }

        template <typename... Args>
        World& addWorld(Args&&... args)
        {
            m_worlds.emplace_back(std::make_unique<World>(std::forward<Args>(args)...));
            m_activeWorlds.setBit(m_worlds.size() - 1);

            return *m_worlds.back();
        }

        void run()
        {
            Logger::debug("[Application] Running...");

            while (runOnce());

            Logger::debug("[Application] Exiting...");
        }

        template <typename FuncT> void run(FuncT&& callback)
        {
            while (runOnce())
                callback(m_timeInfo);
        }

        bool runOnce()
        {
            // ZoneScopedN("Application::runOnce");

            const auto currentTime = std::chrono::system_clock::now();
            m_timeInfo.deltaTime = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
            m_timeInfo.globalTime += m_timeInfo.deltaTime;
            m_lastFrameTime = currentTime;

            m_timeInfo.substepCount = 0;
            m_remainingTime += m_timeInfo.deltaTime;

            while (m_remainingTime >= m_timeInfo.substepTime)
            {
                ++m_timeInfo.substepCount;
                m_remainingTime -= m_timeInfo.substepTime;
            }

            for (std::size_t worldIndex = 0; worldIndex < m_worlds.size(); ++worldIndex)
            {
                if (!m_activeWorlds[worldIndex])
                    continue;

                if (!m_worlds[worldIndex]->update(m_timeInfo))
                    m_activeWorlds.setBit(worldIndex, false);
            }

            // FrameMark;

            return (m_isRunning && !m_activeWorlds.isEmpty());
        }

        void quit() { m_isRunning = false; }
    private:
        std::vector<WorldPtr> m_worlds{};
        Bitset m_activeWorlds{};

        FrameTimeInfo m_timeInfo{ 0.f, 0.f, 0, 0.016666f };
        std::chrono::time_point<std::chrono::system_clock> m_lastFrameTime = std::chrono::system_clock::now();
        float m_remainingTime{};

        bool m_isRunning = true;
    };

} // namespace Rei
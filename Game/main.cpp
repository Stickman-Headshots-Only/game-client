#include "Rei.h"

int main()
{
    Rei::Logger::setLoggingLevel(Rei::LoggingLevel::DEBUG);

    Rei::Application app;
    Rei::World& world = app.addWorld(3);

    
    // auto& render = world.addSystem<Rei::RenderSystem>(1280, 720, "Rei Engine");

    app.run();
    return 0;
}
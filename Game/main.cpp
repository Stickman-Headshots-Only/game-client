#include "Application.h"
#include "Logger.h"

int main()
{
    Rei::Logger::setLoggingLevel(Rei::LoggingLevel::DEBUG);
    Rei::Application app;
    app.run();
    return 0;
}
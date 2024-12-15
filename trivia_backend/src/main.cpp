#include "utils/configLoader/configLoader.h"
#include "constants.h"
#include "server.h"

int main() {
    // clear log file trivia.log
    std::ofstream(LOG_FILE_PATH).close();

    // Load config file, if it doesn't exist, use default values
    Endpoint serverAddress = ConfigLoader::loadConfig(CONFIG_FILE_PATH);

    // Start server
    Server server(serverAddress);
    server.run();

    return 0;
}

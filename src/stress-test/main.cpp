

#include <climits>
#include <cstdlib>

#include "common/config.h"

#include "logging/logging.h"

// Our configuration
Config* config = nullptr;


int main(int argc, char **argv) {

    const char* programName = argv[0];

    // Default to INFO logging
    log_set_level(LOG_INFO);

    // Create the config
    config = new Config(argc, argv);
    if(config->processCommandLine(argc, argv) != 1) {
        warning("processCommandLine() did not return 1");
        config->doHelp(programName);
        exit(0);
    }
}
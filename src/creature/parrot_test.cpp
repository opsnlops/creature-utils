


#include "parrot.h"

#include "log.h"


int main() {

    log_set_level(LOG_DEBUG);

    log_info("testing parrot");

    parrot_value_from_joystick(1, 39024);
    parrot_value_from_joystick(0, 12345);


    return 0;
}
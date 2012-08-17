
#ifndef __DAMARIS_DEFINITION_H
#define __DAMARIS_DEFINITION_H

#include "event/BoundAction.hpp"

extern "C" {

int DC_bind_function(const char* event, Damaris::BoundAction::fun_t f);

}

#endif

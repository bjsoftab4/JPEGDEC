#ifndef ARDUINO_H
#define ARDUINO_H

#if defined(MICROPY_VERSION_MAJOR)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "py/obj.h"
#include "py/objstr.h"
#include "py/objmodule.h"
#include "py/runtime.h"
#include "py/builtin.h"
#define int64_t int32_t
#endif
#endif

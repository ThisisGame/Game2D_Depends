#pragma once

#include"lua.h"

#define LUA_CJSON   "cjson"
LUALIB_API int (luaopen_cjson)(lua_State *l);
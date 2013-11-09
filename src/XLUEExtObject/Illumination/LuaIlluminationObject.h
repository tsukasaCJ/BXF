#ifndef __ILLUMINATIONOBJECT_LUAHOST_H__
#define __ILLUMINATIONOBJECT_LUAHOST_H__

#include "./IlluminationObject.h"

class LuaIlluminationSessionObject
	: public ExtObjLuaHostImplEx<LuaIlluminationSessionObject, IlluminationSessionObject>
{
public:
	static int InvalidSession(lua_State* luaState);
public:
	static const XLLRTGlobalAPI s_szLuaMemberFuncs[];
};

class LuaIlluminationLayerObject
	: public ExtObjLuaHostImplEx<LuaIlluminationLayerObject, IlluminationLayerObject>
{
public:
	static int SetLayerHeight(lua_State* luaState);
	static int GetLayerHeight(lua_State* luaState);
public:

	static const XLLRTGlobalAPI s_szLuaMemberFuncs[];
};

class LuaIlluminantObject
	: public ExtObjLuaHostImplEx<LuaIlluminantObject, IlluminantObject>
{
public:
	static int SetDirectionX(lua_State* luaState);
	static int SetDirectionY(lua_State* luaState);
	static int GetDirectionX(lua_State* luaState);
	static int GetDirectionY(lua_State* luaState);
	static int GetColor(lua_State* luaState);
	static int SetColor(lua_State* luaState);
	static int SetColorId(lua_State* luaState);
public:

	static const XLLRTGlobalAPI s_szLuaMemberFuncs[];
};



#endif
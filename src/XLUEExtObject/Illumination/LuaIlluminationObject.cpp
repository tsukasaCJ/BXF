#include "stdafx.h"
#include "./LuaIlluminationObject.h"


const XLLRTGlobalAPI LuaIlluminationSessionObject::s_szLuaMemberFuncs[] = {
	{"InvalidSession", LuaIlluminationSessionObject::InvalidSession}
	, {NULL, NULL}};
const XLLRTGlobalAPI LuaIlluminationLayerObject::s_szLuaMemberFuncs[] = {
	{"SetLayerHeight",  LuaIlluminationLayerObject::SetLayerHeight}
	, {"GetLayerHeight", LuaIlluminationLayerObject::GetLayerHeight}
	, {NULL, NULL}};
const XLLRTGlobalAPI LuaIlluminantObject::s_szLuaMemberFuncs[] = {
	{"SetDirectionX", LuaIlluminantObject::SetDirectionX}
	, {"SetDirectionY", LuaIlluminantObject::SetDirectionY}
	, {"GetDirectionX", LuaIlluminantObject::GetDirectionX}
	, {"GetDirectionY", LuaIlluminantObject::GetDirectionY}
	, {"GetColor", LuaIlluminantObject::GetColor}
	, {"SetColor", LuaIlluminantObject::SetColor}
	, {"SetColorId", LuaIlluminantObject::SetColorId}
	, {NULL, NULL}};


int LuaIlluminationLayerObject::SetLayerHeight(lua_State* luaState)
{
	IlluminationLayerObject* lpObj = CheckExtObject(luaState, 1);
	int height = lua_tointeger(luaState, 2);
	lpObj->SetLayerHeight(height);
	return 0;
}

int LuaIlluminationLayerObject::GetLayerHeight(lua_State* luaState)
{
	IlluminationLayerObject* lpObj = CheckExtObject(luaState, 1);
	lua_pushinteger(luaState, lpObj->GetLayerHeight());
	return 1;
}

int LuaIlluminationSessionObject::InvalidSession(lua_State* luaState)
{
	IlluminationSessionObject* lpObj = CheckExtObject(luaState, 1);
	lpObj->InvalidSession();
	return 0;
}


int LuaIlluminantObject::SetDirectionX(lua_State* luaState)
{
	IlluminantObject* lpObj = CheckExtObject(luaState, 1);
	int directionX = lua_tointeger(luaState ,2);
	lpObj->SetDirectionX(directionX);
	return 0;
}

int LuaIlluminantObject::SetDirectionY(lua_State* luaState)
{
	IlluminantObject* lpObj = CheckExtObject(luaState, 1);
	int directionY = lua_tointeger(luaState ,2);
	lpObj->SetDirectionY(directionY);
	return 0;
}
int LuaIlluminantObject::GetDirectionX(lua_State* luaState)
{
	IlluminantObject* lpObj = CheckExtObject(luaState, 1);
	lua_pushinteger(luaState, lpObj->GetDirectionX());
	return 1;
}

int LuaIlluminantObject::GetDirectionY(lua_State* luaState)
{
	IlluminantObject* lpObj = CheckExtObject(luaState, 1);
	lua_pushinteger(luaState, lpObj->GetDirectionY());
	return 1;
}

int LuaIlluminantObject::GetColor(lua_State* luaState)
{
	IlluminantObject* lpObj = CheckExtObject(luaState, 1);
	XLUE_PushColor2(luaState, lpObj->GetColor());
	return 1;
}

int LuaIlluminantObject::SetColor(lua_State* luaState)
{
	IlluminantObject* lpObj = CheckExtObject(luaState, 1);
	XL_Color color;
	XLUE_CheckColor2(luaState, 2, &color);
	lpObj->SetColor(color);
	return 0;
}

int LuaIlluminantObject::SetColorId(lua_State* luaState)
{
	IlluminantObject* lpObj = CheckExtObject(luaState, 1);
	lpObj->SetColorId(lua_tostring(luaState, 2));
	return 0;
}
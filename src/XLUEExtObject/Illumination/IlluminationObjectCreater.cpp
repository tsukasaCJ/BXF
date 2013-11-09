#include "stdafx.h"
#include "./IlluminationObjectCreater.h"


IlluminationSessionObjectCreater::IlluminationSessionObjectCreater()
{
}

IlluminationSessionObjectCreater::~IlluminationSessionObjectCreater()
{
}

IlluminationSessionObject* IlluminationSessionObjectCreater::CreateObj(const char* lpObjClass, XLUE_LAYOUTOBJ_HANDLE hObj) 
{
	assert(lpObjClass != NULL);
	assert(hObj != NULL);
	return new IlluminationSessionObject(hObj);
}

void IlluminationSessionObjectCreater::DestroyObj(IlluminationSessionObject* lpObj) 
{
	delete lpObj;
}


IlluminationLayerObjectCreater::IlluminationLayerObjectCreater()
{
}

IlluminationLayerObjectCreater::~IlluminationLayerObjectCreater()
{

}

IlluminationLayerObject* IlluminationLayerObjectCreater::CreateObj(const char* lpObjClass, XLUE_LAYOUTOBJ_HANDLE hObj) 
{
	assert(lpObjClass != NULL);
	assert(hObj != NULL);
	return new IlluminationLayerObject(hObj);
}

void IlluminationLayerObjectCreater::DestroyObj(IlluminationLayerObject* lpObj) 
{
	delete lpObj;
}



IlluminantObjectCreater::IlluminantObjectCreater()
{
}
IlluminantObjectCreater::~IlluminantObjectCreater()
{
}

IlluminantObject* IlluminantObjectCreater::CreateObj(const char* lpObjClass, XLUE_LAYOUTOBJ_HANDLE hObj) 
{
	assert(lpObjClass != NULL);
	assert(hObj != NULL);
	return new IlluminantObject(hObj);
}

void IlluminantObjectCreater::DestroyObj(IlluminantObject* lpObj) 
{
	delete lpObj;
}

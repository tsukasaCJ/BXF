#include "stdafx.h"
#include "./IlluminationObjectRegister.h"
#include "./IlluminationObjectCreater.h"
#include "./IlluminationObjectParser.h"
#include "./LuaIlluminationObject.h"


bool IlluminationObjectRegiter::RegiterIlluminationSessionObject()
{
	

	return ExtObjRegisterHelper<ExtObjType_layoutObj, IlluminationSessionObject, IlluminationSessionObjectCreater, 
		IlluminationSessionObjectParser, LuaIlluminationSessionObject>::Register(EXTCLASSNAME_ILLUMINATIONSESSIONOBJECT, 0);
}

bool IlluminationObjectRegiter::RegiterIlluminationLayerObject()
{
	unsigned long attribute = ExtObjAttribute_clipsens;

	return ExtObjRegisterHelper<ExtObjType_renderableObj, IlluminationLayerObject, IlluminationLayerObjectCreater, 
		IlluminationLayerObjectParser, LuaIlluminationLayerObject>::Register(EXTCLASSNAME_ILLUMINATIONLAYEROBJECT, attribute);
}



bool IlluminationObjectRegiter::RegiterIlluminantObject()
{
	return ExtObjRegisterHelper<ExtObjType_layoutObj, IlluminantObject, IlluminantObjectCreater, 
		IlluminantObjectParser, LuaIlluminantObject>::Register(EXTCLASSNAME_ILLUMINANTOBJECT, 0);
}


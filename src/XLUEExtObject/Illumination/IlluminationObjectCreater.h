#ifndef __ILLUMINATIONOBJECT_CREATER_H__
#define __ILLUMINATIONOBJECT_CREATER_H__

#include "./IlluminationObject.h"

class IlluminationSessionObjectCreater
	: public ExtObjCreatorImpl<IlluminationSessionObject>
{
public:
	IlluminationSessionObjectCreater();
	~IlluminationSessionObjectCreater();

public:
	virtual IlluminationSessionObject* CreateObj(const char* lpObjClass, XLUE_LAYOUTOBJ_HANDLE hObj) ;
	virtual void DestroyObj(IlluminationSessionObject* lpObj) ;
};

class IlluminationLayerObjectCreater
	: public ExtObjCreatorImpl<IlluminationLayerObject>
{
public:
	IlluminationLayerObjectCreater();
	~IlluminationLayerObjectCreater();

public:
	virtual IlluminationLayerObject* CreateObj(const char* lpObjClass, XLUE_LAYOUTOBJ_HANDLE hObj) ;
	virtual void DestroyObj(IlluminationLayerObject* lpObj) ;
};


class IlluminantObjectCreater
	: public ExtObjCreatorImpl<IlluminantObject>
{
public:
	IlluminantObjectCreater();
	~IlluminantObjectCreater();

public:
	virtual IlluminantObject* CreateObj(const char* lpObjClass, XLUE_LAYOUTOBJ_HANDLE hObj) ;
	virtual void DestroyObj(IlluminantObject* lpObj) ;
};




#endif //__ILLUMINATIONOBJECT_CREATER_H__
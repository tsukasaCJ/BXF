#ifndef __ILLUMINATIONOBJECT_REGISTER_H__
#define __ILLUMINATIONOBJECT_REGISTER_H__

#include "./IlluminationObject.h"


class IlluminationObjectRegiter
{
public:
	static bool RegiterIlluminationSessionObject();
	static bool RegiterIlluminationLayerObject();
	static bool RegiterIlluminantObject();
};

#endif //__ILLUMINATIONOBJECT_REGISTER_H__
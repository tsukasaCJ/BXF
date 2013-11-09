#ifndef __ILLUMINATIONOBJECT_PARSER_H__
#define __ILLUMINATIONOBJECT_PARSER_H__

#include "./IlluminationObject.h"


class IlluminationSessionObjectParser
	: public ExtObjParserImpl<IlluminationSessionObject>
{
public:
	IlluminationSessionObjectParser();
	~IlluminationSessionObjectParser();

private:
	virtual bool ParserAttribute(IlluminationSessionObject* lpObj, const char* key, const char* value);
};

class IlluminationLayerObjectParser
	: public ExtObjParserImpl<IlluminationLayerObject>
{
public:
	IlluminationLayerObjectParser();
	~IlluminationLayerObjectParser();

private:
	virtual bool ParserAttribute(IlluminationLayerObject* lpObj, const char* key, const char* value);
};


class IlluminantObjectParser
	: public ExtObjParserImpl< IlluminantObject>
{
public:
	IlluminantObjectParser();
	~IlluminantObjectParser();

private:
	virtual bool ParserAttribute( IlluminantObject* lpObj, const char* key, const char* value);
};




#endif //__ILLUMINATIONOBJECT_PARSER_H__
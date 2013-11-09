#include "stdafx.h"
#include "./IlluminationObjectParser.h"

IlluminationSessionObjectParser::IlluminationSessionObjectParser()
{
}

IlluminationSessionObjectParser::~IlluminationSessionObjectParser()
{

}

bool IlluminationSessionObjectParser::ParserAttribute(IlluminationSessionObject* lpObj, const char* key, const char* value)
{
	assert(lpObj);
	assert(key);
	assert(value);

	/*if (0 == ::strcmp(key, "ZorderHeightRate"))
	{
		lpObj->SetZorderHeightRate(static_cast<float>(::atof(value)));
	}*/
	return true;
}




IlluminationLayerObjectParser::IlluminationLayerObjectParser()
{
}

IlluminationLayerObjectParser::~IlluminationLayerObjectParser()
{

}

bool IlluminationLayerObjectParser::ParserAttribute(IlluminationLayerObject* lpObj, const char* key, const char* value)
{
	assert(lpObj);
	assert(key);
	assert(value);

	if (0 == ::strcmp(key, "LayerHeight"))
	{
		lpObj->SetLayerHeight(::atoi(value));
	}
	return true;
}


IlluminantObjectParser::IlluminantObjectParser()
{
}
	
IlluminantObjectParser::~IlluminantObjectParser()
{

}

bool IlluminantObjectParser::ParserAttribute( IlluminantObject* lpObj, const char* key, const char* value)
{
	assert(lpObj);
	assert(key);
	assert(value);

	bool ret = true;
	if (0 == ::strcmp(key, "DirectionX"))
	{
		lpObj->SetDirectionX(::atoi(value));
	}
	else if(0 == ::strcmp(key, "DirectionY"))
	{
		lpObj->SetDirectionY(::atoi(value));
	}
	else if (0 == ::strcmp(key, "Color"))
	{
		std::string stdValue(value);
		unsigned int commaPos[3] = {0,0,0};
		unsigned int offset = 0;
		commaPos[0] = static_cast<unsigned int>(stdValue.find(',',offset));
		if(commaPos[0] == -1)
		{
			ret = false;
		}

		for(unsigned int ix = 0;ix < 3; ++ix)
		{
			commaPos[ix] = static_cast<unsigned int>(stdValue.find(',',offset));
			offset = commaPos[ix] + 1;
			if(commaPos[ix] == -1)
			{
				assert(0);
				ret = false;
			}
		}
		unsigned char B = static_cast<unsigned char>(::atoi(stdValue.substr(0,commaPos[0]).c_str()));
		unsigned char G = static_cast<unsigned char>(::atoi(stdValue.substr(commaPos[0] + 1,commaPos[1] - commaPos[0] - 1).c_str()));
		unsigned char R = static_cast<unsigned char>(::atoi(stdValue.substr(commaPos[1] + 1,commaPos[2] - commaPos[1] - 1).c_str()));
		unsigned char A = static_cast<unsigned char>(::atoi(stdValue.substr(commaPos[2] + 1,stdValue.size() - commaPos[2] - 1).c_str()));
		lpObj->SetColor(XLCOLOR_BGRA(B,G,R,A));
	}
	else if (0 == ::strcmp(key, "ColorId"))
	{
		lpObj->SetColorId(value);
	}
	return ret;
}

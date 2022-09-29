#include <tweakbar.h>

void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
{
  // Copy the content of souceString handled by the AntTweakBar library to destinationClientString handled by your application
  destinationClientString = sourceLibraryString;
}

void TW_CALL CopyStdStringToLibarary(std::string& destinationLibraryString, const std::string& sourceClientString)
{
  // Copy the content of souceString handled by the AntTweakBar library to destinationClientString handled by your application
  destinationLibraryString = sourceClientString;
}

template <> TwType GetType< unsigned char >(unsigned char var)	{ return TW_TYPE_UINT8; }
template <> TwType GetType< unsigned >(unsigned var)			{ return TW_TYPE_UINT32; }
template <> TwType GetType< int >(int var)						{ return TW_TYPE_INT32; }
template <> TwType GetType< float >(float var)					{ return TW_TYPE_FLOAT; }
template <> TwType GetType< std::string >(std::string var)		{ return TW_TYPE_STDSTRING; }
template <> TwType GetType< char* >(char* var)					{ return TW_TYPE_CDSTRING; }
template <> TwType GetType< vec2 >(vec2 var)					{ return TW_TYPE_VEC2; }
template <> TwType GetType< b2vec2 >(b2vec2 var)				{ return TW_TYPE_B2VEC2; }


#include <Transform.h>


namespace TB_Transform
{		
	void TW_CALL SetRotation(const void* data, void* transform)
	{
		float z = *(const float*)data;
		Transform* t = (Transform*)transform;
		t->SetAngle(z);
	}
	void TW_CALL GetRotation(void* data, void* transform)
	{
		Transform* t = (Transform*)transform;
		*(float*)data = t->GetAngle();
	}
}
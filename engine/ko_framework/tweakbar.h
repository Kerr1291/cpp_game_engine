#ifndef TWEAKBAR_SYSTEM_H
#define TWEAKBAR_SYSTEM_H

#include <globals.h>

//Type Conversions

template <typename T> TwType GetType(T var){ return TW_TYPE_CHAR; }
template <> TwType GetType< unsigned char >(unsigned char var);
template <> TwType GetType< unsigned >(unsigned var);
template <> TwType GetType< int >(int var);
template <> TwType GetType< float >(float var);
template <> TwType GetType< std::string >(std::string var);
template <> TwType GetType< char* >(char* var);
template <> TwType GetType< vec2 >(vec2 var);
template <> TwType GetType< b2vec2 >(b2vec2 var);

//Variable Registration Macros

#define REGISTER_RW( CLASS, var ) \
	TwAddVarRW(_t, (std::string( #var ) + GetIdString()).c_str(), GetType(var), &var, (" group=" + std::string( #CLASS ) + " label=" + std::string( #var )).c_str() ); 
#define REGISTER_RO( CLASS, var ) \
	TwAddVarRO(_t, (std::string( #var ) + GetIdString()).c_str(), GetType(var), &var, (" group=" + std::string( #CLASS ) + " label=" + std::string( #var )).c_str() ); 

//TweakBar string function
void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString);

//Base class

/*
class TBWrapper
{
    public:
		TBWrapper(const std::string& name, unsigned unique_id)
		{
			Init(TwNewBar(name.c_str()), unique_id);
			TwCopyStdStringToClientFunc(CopyStdStringToClient); 
		}

		virtual void RegisterTBVariables() = 0;

		const std::string& GetName() { return tb_name; }
		const unsigned GetId() { return tb_unique_id; }
		const std::string& GetIdString() { return tb_unique_id_string; }

    protected:
        TwBar* _t;

    private:
			
		void Init(TwBar* t, unsigned unique_id){
            _t = t;
			tb_name = std::string( TwGetBarName(_t) );
			tb_unique_id = unique_id;

			std::stringstream tostr;
			tostr << tb_unique_id;
			tb_unique_id_string = tostr.str();

			TwDefine((" " + tb_name + " iconified=true ").c_str());
		}

		std::string tb_name;
		unsigned tb_unique_id;
		std::string tb_unique_id_string;
};
*/

#endif
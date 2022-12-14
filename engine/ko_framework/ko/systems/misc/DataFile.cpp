#include <DataFile.h>
#include <iostream>
#include <list>

const std::string COMMENT_BLOCK_START_A = "[";
const std::string COMMENT_BLOCK_END_A   = "]";
const std::string COMMENT_LINE_A        = "#";
const std::string COMMENT_LINE_B        = ";";
const std::string TOKEN_ASSIGNMENT_A    = "=";
const std::string TOKEN_ASSIGNMENT_B    = ":";
const std::string TOKEN_DELIMITER_A     = " ";
const std::string TOKEN_DELIMITER_B     = "\n";
const std::string TOKEN_DELIMITER_C     = "\t";
const std::string TOKEN_DELIMITER_D     = ",";
#ifdef _WIN32
const std::string TOKEN_DELIMITER_E     = "\r";
#else
const std::string TOKEN_DELIMITER_E     = " ";
#endif
const std::string TOKEN_STRING_A        = "\"";
const std::string TOKEN_STRING_B        = "\'";
const std::string DATA_BLOCK_START_A    = "{";
const std::string DATA_BLOCK_END_A      = "}";


bool IsDelimiter(const std::string& c)
{
    if( c == TOKEN_DELIMITER_A
    || c == TOKEN_DELIMITER_B
    || c == TOKEN_DELIMITER_C
    || c == TOKEN_DELIMITER_D
	|| c == TOKEN_DELIMITER_E
    ) return true;
    return false;
}

bool IsSpecialToken(const std::string& c)
{
    if( c == TOKEN_ASSIGNMENT_A
    || c == TOKEN_ASSIGNMENT_B
    || c == TOKEN_DELIMITER_A
    || c == TOKEN_DELIMITER_B
    || c == TOKEN_DELIMITER_C
    || c == TOKEN_DELIMITER_D
    || c == TOKEN_DELIMITER_E
    || c == DATA_BLOCK_START_A
    || c == DATA_BLOCK_END_A
    ) return true;
    return false;
}

bool IsAssignmentToken(const std::string& c)
{
    if( c == TOKEN_ASSIGNMENT_A
    || c == TOKEN_ASSIGNMENT_B
    ) return true;
    return false;
}


typedef void (*parser_state)(Parser&);

void Start(Parser& parser);
void CommentBlock(Parser& parser);
void CommentLine(Parser& parser);
void ReadString(Parser& parser);
void ReadField(Parser& parser);


class Parser {
    public:
    Parser(std::fstream& file, DataFile& data, bool JSON)
        :_file(file)
        ,_data(data)
        ,_syntax_error(false)
        ,_complete(false)
        ,state(0)
        ,previous_state(0)
        ,_is_JSON(JSON)
        {}
    ~Parser(){_file.close();}

    void Run();
    bool Success() { return !_syntax_error; }

    private:
    bool PeekChar(char& c);
    bool ReadChar(char& c);


    friend void Start(Parser& parser);
    friend void CommentBlock(Parser& parser);
    friend void CommentLine(Parser& parser);
    friend void ReadString(Parser& parser);
    friend void ReadField(Parser& parser);


    std::fstream& _file;
    DataFile& _data;
    std::string token_buffer;
    std::string key;
    std::vector< std::string > values;
    std::list< SerializedObject* > current_object;

    bool _syntax_error;
    bool _complete;
    parser_state state;
    parser_state previous_state;
    const bool _is_JSON;
};
bool Parser::PeekChar(char& c)
{
    if( _file.good() )
    {
        _file.get( c );
        _file.unget();
        return true;
    }
    return false;
}
bool Parser::ReadChar(char& c)
{
    if( _file.good() )
    {
        _file.get( c );
        if( _file.good() )
            return true;
    }
    return false;
}

void CommentBlock(Parser& parser)
{
    if( !parser._file.good() )
    {
        std::cerr<<"Error while parsing comment block. End of file reached. ']' expected.\n";
        parser._syntax_error = true;
        parser._complete = true;
        return;
    }

    char c;
    std::string token;
    parser.ReadChar( c );
    token=""; token.push_back( c );
    if( token == COMMENT_BLOCK_END_A )
    {
        parser.state = parser.previous_state;
    }
}

void CommentLine(Parser& parser)
{
    char c;
    std::string token;
    parser.ReadChar( c );
    token=""; token.push_back( c );
    if( token == "\n" )
    {
        parser.state = parser.previous_state;
    }
}

void ReadString(Parser& parser)
{
    if( !parser._file.good() )
    {
        std::cerr<<"Error while parsing string. End of file reached. End-quotation mark expected.\n";
        parser._syntax_error = true;
        parser._complete = true;
        return;
    }

    char c;
    parser.PeekChar( c );
    std::string token;
    token=""; token.push_back( c );
    //did we complete the string?
    if( token == TOKEN_STRING_A || token == TOKEN_STRING_B )
    {
        parser.ReadChar( c ); //munch the end token
        parser.state = parser.previous_state;
        return;
    }

    parser.ReadChar( c );
    parser.token_buffer.push_back(c);
}

void ReadField(Parser& parser)
{
    if( !parser._file.good() )
    {
        std::cerr<<"Error while parsing field. End of file reached.\n";
        std::cerr<<"End of data block in object "<<parser.current_object.back()->_name <<". '}' expected.\n";
        parser._syntax_error = true;
        parser._complete = true;
        return;
    }

    //if we parse in a "null" token ever, kill it if we're parsing JSON
    if( parser._is_JSON && parser.token_buffer == "null" )
    {
        parser.token_buffer="";
    }

    char c;
    std::string token;
    parser.PeekChar(c);

    token=""; token.push_back( c );
    if( token == COMMENT_BLOCK_START_A )
    {
        //just munch and ignore block comment delimiters for JSON parsing
        if( parser._is_JSON )
        {
            parser.ReadChar( c );
            return;
        }

        parser.previous_state = parser.state;
        parser.state = CommentBlock;
        return;
    }
    else if( parser._is_JSON && token == COMMENT_BLOCK_END_A )
    {
        parser.ReadChar( c );
        return;
    }
    else if( token == COMMENT_LINE_A || token == COMMENT_LINE_B )
    {
        parser.previous_state = parser.state;
        parser.state = CommentLine;
        return;
    }
    else if( token == DATA_BLOCK_START_A )
    {
        if( parser.token_buffer.empty() )
        {
            if( !parser._is_JSON )
            {
                std::cerr<<"Error while parsing file. Attempted assgnment to nested object '"<<parser.key<<"'encountered.\n";
                std::cerr<<"Token will be ignored and parsing will continue.\n";
            }
            parser.token_buffer = parser.key;
            parser.key = "";
        }

        if( parser.current_object.empty() )
        {
            parser._data.objects[""].AddValue(parser.key, parser.values);
        }
        else
        {
            parser.current_object.back()->AddValue(parser.key, parser.values);
        }
        parser.state = Start;
        parser.values.clear();
        parser.values.resize(0);
        return;
    }

    token="";
    bool good = parser.ReadChar( c );
    if(good)
        token.push_back( c );

    if( token == TOKEN_STRING_A || token == TOKEN_STRING_B )
    {
        //may only reach here if we're not reading a string and we hit the string token
        parser.previous_state = parser.state;
        parser.state = ReadString;
        return;
    }
    else if( IsAssignmentToken( token ) )
    {
        if( parser.current_object.empty() )
        {
            parser._data.objects[""].AddValue(parser.key, parser.values);
        }
        else
        {
            parser.current_object.back()->AddValue(parser.key, parser.values);
        }
        parser.values.clear();
        parser.values.resize(0);
        parser.key = parser.token_buffer;
        parser.token_buffer = "";
        return;
    }
    else if( IsDelimiter(token) )
    {
        if( parser.token_buffer.empty() )
            return;

        char next = 0;
        if( parser.PeekChar( next ) )
        {
            token=""; token.push_back( next );
            if( IsSpecialToken( token ) )
                return;
        }

        parser.values.push_back( parser.token_buffer );
        parser.token_buffer = "";
        return;
    }
    else if( token == DATA_BLOCK_END_A )
    {
        if( parser.token_buffer.empty() && parser.values.empty() )
        {
            std::cerr<<"Error while parsing field. End of object reached.\n";
            std::cerr<<"End of data block in object "<<parser.current_object.back()->_name <<". value expected for field '"<<parser.key <<"'.\n";
            std::cerr<<"Exiting bad object and continuing parse.\n";
            parser.token_buffer = "";
            parser.values.clear();
            parser.values.resize(0);
            parser.current_object.pop_back();
            parser.state = Start;
            parser._syntax_error = true;
            return;
        }

        if( !parser.token_buffer.empty() )
        {
            parser.values.push_back( parser.token_buffer );
            parser.token_buffer = "";
        }

        if( parser.current_object.empty() )
        {
            parser._data.objects[""].AddValue(parser.key, parser.values);
        }
        else
        {
            parser.current_object.back()->AddValue(parser.key, parser.values);
        }
        parser.values.clear();
        parser.values.resize(0);
        parser.current_object.pop_back();
        parser.state = Start;
        return;
    }

    if(good)
        parser.token_buffer.push_back(c);
}

void Start(Parser& parser)
{
    if( !parser._file.good() )
    {
        if( !parser.current_object.empty() )
        {
            std::cerr<<"Error while parsing object. End of file reached.\n";
            std::cerr<<"End of data block in object "<<parser.current_object.back()->_name <<". '}' expected.\n";
            parser._syntax_error = true;
        }

        parser._complete = true;
        return;
    }

    //if we parse in a "null" token ever, kill it if we're parsing JSON
    if( parser._is_JSON && parser.token_buffer == "null" )
    {
        parser.token_buffer="";
    }

    char c;
    std::string token;
    parser.PeekChar(c);

    token=""; token.push_back( c );
    if( token == COMMENT_BLOCK_START_A )
    {
        //just munch and ignore block comment delimiters for JSON parsing
        if( parser._is_JSON )
        {
            parser.ReadChar( c );
            return;
        }

        parser.token_buffer = "";
        parser.previous_state = parser.state;
        parser.state = CommentBlock;
        return;
    }
    else if( parser._is_JSON && token == COMMENT_BLOCK_END_A )
    {
        parser.ReadChar( c );
        return;
    }
    else if( token == COMMENT_LINE_A || token == COMMENT_LINE_B )
    {
        parser.token_buffer = "";
        parser.previous_state = parser.state;
        parser.state = CommentLine;
        return;
    }

    token="";
    bool good = parser.ReadChar( c );
    if(good)
        token.push_back( c );

    if( IsDelimiter(token) )
    {
        return;
    }
    else if( token == DATA_BLOCK_START_A )
    {
        SerializedObject new_object(parser.token_buffer);
        SerializedObject* pobj = 0;
        bool add_success = false;
        if( parser.current_object.empty() )
        {
            add_success = parser._data.AddObject(parser.token_buffer, new_object);
            pobj = &parser._data.objects[ parser.token_buffer ];
        }
        else
        {
            add_success = parser.current_object.back()->AddObject(parser.token_buffer, new_object);
            pobj = &parser.current_object.back()->objects[ parser.token_buffer ];
        }

        if( !add_success )
        {
            if( parser.token_buffer.empty() )
            {
                if( parser.current_object.empty() )
                    std::cerr<<"Warning: Anonymous object already exists. \nAnything inside this object will be added to the existing anonymous object.\n";
                else
                    std::cerr<<"Warning: Anonymous object already exists. \nAnything inside this object will be added to the existing anonymous object inside object: "<<parser.current_object.back()->_name <<".\n";
            }
            else
                std::cerr<<"Warning: Object "<<parser.token_buffer <<"already exists. Anything inside this object will be added to the existing object.\n";
        }

        parser.current_object.push_back( pobj );
        parser.token_buffer = "";
        return;
    }
    else if( IsAssignmentToken( token ) )
    {
        if( parser.current_object.empty() )
        {
            if( !parser._is_JSON )
            {
                std::cerr<<"Error while parsing file. Attempted assgnment to object '"<<parser.token_buffer<<"'encountered.\n";
                std::cerr<<"Token will be ignored and parsing will continue.\n";
            }
            return;
        }

        parser.values.clear();
        parser.values.resize(0);
        parser.key = parser.token_buffer;
        parser.token_buffer = "";
        parser.previous_state = parser.state;
        parser.state = ReadField;
        return;
    }
    else if( token == DATA_BLOCK_END_A )
    {
        parser.current_object.pop_back();
        return;
    }
    else if( token == TOKEN_STRING_A || token == TOKEN_STRING_B )
    {
        //may only reach here if we're not reading a string and we hit the string token
        parser.previous_state = parser.state;
        parser.state = ReadString;
        return;
    }

    if(good)
        parser.token_buffer.push_back(c);
}

void Parser::Run()
{
    state = Start;
    while( !_complete )
    {
        state(*this);
    }
}

bool DataFile::Load(const std::string& filename, bool JSON)
{
    std::fstream file;
    file.open( filename.c_str(), std::ios::in | std::ios::binary );
    if( !file.is_open() )
    {
        std::cout<<"ERROR Opening "<<filename <<".\n";
        return false;
    }
    Parser parser(file, *this, JSON);
    parser.Run();
    if( parser.Success() )
        return true;
    return false;
}




SerializedObject::SerializedObject()
:_name("")
{
}

SerializedObject::SerializedObject(const std::string& name)
:_name(name)
{
}

DataFile::DataFile()
{
}


template <>
bool SerializedObject::GetValue<unsigned char>(const std::string& key, unsigned char& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        //value = Serialize( dictionary[ key ] );
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<char>(const std::string& key, char& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        //value = Serialize( dictionary[ key ] );
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<char>(const std::string& key, char* value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        for(size_t i = 0; i < dictionary[ key ].size(); ++i)
        {
            std::stringstream data;
            data << dictionary[ key ][i];
            data >> value[i];
        }
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<int>(const std::string& key, int& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        //value = Serialize( dictionary[ key ] );
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<int>(const std::string& key, int* value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        for(size_t i = 0; i < dictionary[ key ].size(); ++i)
        {
            std::stringstream data;
            data << dictionary[ key ][i];
            data >> value[i];
        }
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<unsigned int>(const std::string& key, unsigned int* value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        for(size_t i = 0; i < dictionary[ key ].size(); ++i)
        {
            std::stringstream data;
            data << dictionary[ key ][i];
            data >> value[i];
        }
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<unsigned int>(const std::string& key, unsigned int& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}


template <>
bool SerializedObject::GetValue<long>(const std::string& key, long& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<short>(const std::string& key, short& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<float>(const std::string& key, float& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<float>(const std::string& key, float* value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        for(size_t i = 0; i < dictionary[ key ].size(); ++i)
        {
            std::stringstream data;
            data << dictionary[ key ][i];
            data >> value[i];
        }
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<bool>(const std::string& key, bool& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        std::stringstream data;
        data << dictionary[ key ][0];
        data >> value;
        return true;
    }
    return false;
}

template <>
bool SerializedObject::GetValue<std::string>(const std::string& key, std::string& value)
{
    if( dictionary.find( key ) != dictionary.end() )
    {
        value = dictionary[ key ][0];
        return true;
    }
    return false;
}

void SerializedObject::AddValue(const std::string& key, const std::string& value)
{
    if( dictionary.find( key ) == dictionary.end() )
    {
        std::vector<std::string> v; v.push_back( value );
        dictionary[ key ] = v;
    }
    else
        std::cerr<<"Warning: Key "<<key <<" already exists in object "<<_name <<". Value ignored.\n";
}

void SerializedObject::AddValue(const std::string& key, std::vector< std::string > value)
{
    if( dictionary.find( key ) == dictionary.end() )
    {
        dictionary[ key ] = value;
    }
    else
        std::cerr<<"Warning: Key "<<key <<" already exists in object "<<_name <<". Value ignored.\n";
}



bool SerializedObject::AddObject(const std::string& key, SerializedObject& value)
{
    if( objects.find( key ) == objects.end() )
    {
        objects[ key ] = value;
        return true;
    }
    return false;
}


bool DataFile::AddObject(const std::string& key, SerializedObject& value)
{
    if( objects.find( key ) == objects.end() )
    {
        objects[ key ] = value;
        return true;
    }
    return false;
}



/*
struct t
{
    t(){
        DataFile data;
        data.Load("./test_data.txt", true);
        SerializedObject obj;
        data.GetObject("GO-01", obj);
        std::cout<<" result = " <<obj.HasObject("Tfm") <<"\n";
        SerializedObject tfm;
        obj.GetObject("Tfm", tfm);
        float pos[3];
        tfm.GetValue("pos",pos);
        std::cout << pos[0] <<"," << pos[1] <<"," << pos[2] <<"\n";

        SerializedObject obja;
        SerializedObject objb;
        data.GetObject("", obja);
        obja.GetObject("menu", objb);
        objb.GetObject("items", obja);
        float pos2[3] = {0,0,0};
        std::string id ="";
        obja.GetValue("id",id);
        std::cout << pos2[0] <<"," << pos2[1] <<"," << pos2[2] <<"\n";
        std::cout << id <<" <- id\n";
}
};
static t tt;
*/




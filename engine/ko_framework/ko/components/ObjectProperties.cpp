#include <ObjectProperties.h>

#include <Editor.h>

ObjectProperties::ObjectProperties()
{
	parent_bar=0;
	tb_parent_name="";
}

ObjectProperties::~ObjectProperties()
{
}

void ObjectProperties::AddProperty(const std::string& prop_name, float default_value)
{
        if( properties.find( prop_name ) == properties.end() )
		{
                properties[ prop_name ] = default_value;

				if( parent_bar )
				{
					std::string var_name = prop_name + tb_parent_name;
					TwAddVarRW(parent_bar, var_name.c_str(), TW_TYPE_FLOAT, &properties[ prop_name ], (" group='"+tb_parent_name+"' label='"+prop_name+"'").c_str() );
					External::editor->RefreshObjectEditor();
				}
		}
}

void ObjectProperties::RemoveProperty(const std::string& prop_name)
{
        if( properties.find( prop_name ) != properties.end() )
		{
			if( parent_bar )
				TwRemoveVar(parent_bar, (prop_name+tb_parent_name).c_str());

            properties.erase( prop_name );
		}
}

truth ObjectProperties::HasProperty(const std::string& prop_name)
{
        if( properties.find( prop_name ) != properties.end() )
                return 1;
        return 0;
}

float ObjectProperties::GetProperty(const std::string& prop_name)
{
        if( properties.find( prop_name ) != properties.end() )
                return properties[ prop_name ];
        float nan1 = 0.0f;
        float nan2 = 0.0f;
        return (nan1/nan2);
}

void ObjectProperties::SetProperty(const std::string& prop_name, float new_value)
{
        if( properties.find( prop_name ) != properties.end() )
                properties[ prop_name ] = new_value;
}

void ObjectProperties::ModifyProperty(const std::string& prop_name, float mod_value)
{
        if( properties.find( prop_name ) != properties.end() )
                properties[ prop_name ] += mod_value;
}



void ObjectProperties::AddFlag(const std::string& flag_name, truth default_value)
{
        if( flags.find( flag_name ) == flags.end() )
		{
                flags[ flag_name ] = default_value;
				
				if( parent_bar )
				{
					std::string var_name = flag_name + tb_parent_name;
					TwAddVarRW(parent_bar, var_name.c_str(), TW_TYPE_UINT8, &flags[ flag_name ], (" group='"+tb_parent_name+"' label='"+flag_name+"' min=0 max=1").c_str() );
					External::editor->RefreshObjectEditor();
				}
		}
}

void ObjectProperties::RemoveFlag(const std::string& flag_name)
{
        if( flags.find( flag_name ) != flags.end() )
		{
			if( parent_bar )
				TwRemoveVar(parent_bar, (flag_name+tb_parent_name).c_str());

            flags.erase( flag_name );
		}
}

truth ObjectProperties::HasFlag(const std::string& flag_name)
{
        if( flags.find( flag_name ) != flags.end() )
                return 1;
        return 0;
}

truth ObjectProperties::GetFlag(const std::string& flag_name)
{
        if( flags.find( flag_name ) != flags.end() )
                return flags[ flag_name ];
        return 0;
}

void ObjectProperties::SetFlag(const std::string& flag_name, truth new_value)
{
        if( flags.find( flag_name ) != flags.end() )
                flags[ flag_name ] = new_value;
}

void ObjectProperties::ToggleFlag(const std::string& flag_name)
{
        if( flags.find( flag_name ) != flags.end() )
                flags[ flag_name ] = (flags[ flag_name ] ? 0 : 1);
}

namespace TB_ObjectProperties
{		
	static std::string tbPropertyName;
	static std::string tbFlagName;
	
	void TW_CALL tbAddProperty(void* properties)
	{
		if( tbPropertyName.empty() )
			return;

		ObjectProperties* o = (ObjectProperties*)properties;
		o->AddProperty(tbPropertyName,0.0f);
	}
	
	void TW_CALL tbRemoveProperty(void* properties)
	{
		if( tbPropertyName.empty() )
			return;

		ObjectProperties* o = (ObjectProperties*)properties;
		o->RemoveProperty(tbPropertyName);
	}

	void TW_CALL tbAddFlag(void* properties)
	{
		if( tbFlagName.empty() )
			return;

		ObjectProperties* o = (ObjectProperties*)properties;
		o->AddFlag(tbFlagName, 0);
	}
	
	void TW_CALL tbRemoveFlag(void* properties)
	{
		if( tbFlagName.empty() )
			return;

		ObjectProperties* o = (ObjectProperties*)properties;
		o->RemoveFlag(tbFlagName);
	}
}

void ObjectProperties::ExposeToTweakbar( TwBar* bar, const std::string& parent )
{
	parent_bar = bar;
	tb_parent_name=parent;

	std::map< std::string, float >::iterator p_iter = properties.begin();
    std::map< std::string, truth >::iterator f_iter = flags.begin();
	
	TwAddSeparator(bar, ("buttons"+parent).c_str(), (" group='"+parent+"'").c_str());

	TwAddVarRW(bar, ("NewPropertyName"+parent).c_str(), TW_TYPE_STDSTRING, &TB_ObjectProperties::tbPropertyName, (" group='"+parent+"' label='New Property Name:'").c_str() );
	TwAddButton(bar, ("AddNewProperty"+parent).c_str(), TB_ObjectProperties::tbAddProperty, this, (" group='"+parent+"' label='Add New Property' ").c_str() );
	TwAddButton(bar, ("RemoveProperty"+parent).c_str(), TB_ObjectProperties::tbRemoveProperty, this, (" group='"+parent+"' label='Remove Property' ").c_str() );
	
	TwAddVarRW(bar, ("NewFlagName"+parent).c_str(), TW_TYPE_STDSTRING, &TB_ObjectProperties::tbFlagName, (" group='"+parent+"' label='New Flag Name:'").c_str() );
	TwAddButton(bar, ("AddNewFlag"+parent).c_str(), TB_ObjectProperties::tbAddFlag, this, (" group='"+parent+"' label='Add New Flag' ").c_str() );
	TwAddButton(bar, ("RemoveFlag"+parent).c_str(), TB_ObjectProperties::tbRemoveFlag, this, (" group='"+parent+"' label='Remove Flag' ").c_str() );

	TwAddSeparator(bar, ("variables"+parent).c_str(), (" group='"+parent+"'").c_str());

	for(; p_iter != properties.end(); ++p_iter)
	{
		std::string var_name = p_iter->first + parent;
		TwAddVarRW(bar, var_name.c_str(), TW_TYPE_FLOAT, &p_iter->second, (" group='"+parent+"' label='"+p_iter->first+"'").c_str() );
	}
	for(; f_iter != flags.end(); ++f_iter)
	{
		std::string var_name = f_iter->first + parent;
		TwAddVarRW(bar, var_name.c_str(), TW_TYPE_UINT8, &f_iter->second, (" group='"+parent+"' label='"+f_iter->first+"' min=0 max=1").c_str() );
	}
}

void ObjectProperties::RemoveFromTweakbar( TwBar* bar, const std::string& parent )
{
	std::map< std::string, float >::iterator p_iter = properties.begin();
    std::map< std::string, truth >::iterator f_iter = flags.begin();

	TwRemoveVar(bar, ("buttons"+parent).c_str());
	TwRemoveVar(bar, ("variables"+parent).c_str());

	TwRemoveVar(bar, ("NewPropertyName"+parent).c_str());
	TwRemoveVar(bar, ("AddNewProperty"+parent).c_str());
	TwRemoveVar(bar, ("RemoveProperty"+parent).c_str());
	TwRemoveVar(bar, ("NewFlagName"+parent).c_str());
	TwRemoveVar(bar, ("AddNewFlag"+parent).c_str());
	TwRemoveVar(bar, ("RemoveFlag"+parent).c_str());

	for(; p_iter != properties.end(); ++p_iter)
	{
		std::string var_name = p_iter->first + parent;
		TwRemoveVar(bar, var_name.c_str());
	}
	for(; f_iter != flags.end(); ++f_iter)
	{
		std::string var_name = f_iter->first + parent;
		TwRemoveVar(bar, var_name.c_str());
	}
	parent_bar=0;
	tb_parent_name="";
}
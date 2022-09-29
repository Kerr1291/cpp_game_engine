#ifndef OBJECTPROPERTIES_H
#define OBJECTPROPERTIES_H

#include <globals.h>
#include <tweakbar.h>

class ObjectProperties
{
        public:
                ObjectProperties();
                ~ObjectProperties();

                void AddProperty(const std::string& prop_name, float default_value);
                void RemoveProperty(const std::string& prop_name);
                truth HasProperty(const std::string& prop_name);
                float GetProperty(const std::string& prop_name);
                void SetProperty(const std::string& prop_name, float new_value);
                void ModifyProperty(const std::string& prop_name, float mod_value);


                void AddFlag(const std::string& flag_name, truth default_value);
                void RemoveFlag(const std::string& flag_name);
                truth HasFlag(const std::string& flag_name);
                truth GetFlag(const std::string& flag_name);
                void SetFlag(const std::string& flag_name, truth new_value);
                void ToggleFlag(const std::string& flag_name);

				void ExposeToTweakbar( TwBar* bar, const std::string& parent );
				void RemoveFromTweakbar( TwBar* bar, const std::string& parent );
       
        private:

				TwBar* parent_bar;
				std::string tb_parent_name;

                std::map< std::string, float > properties;
                std::map< std::string, truth > flags;
};



#endif

#ifndef CONFIG_H
#define CONFIG_H

#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }
#endif

#include <string>
#include <vector>

#include "../xml/tinyxml2.h"

class config_entity {
    public:
        std::string id;
        std::string data;
        std::string category;

        bool operator < (const config_entity& obj) const {
            return ( category < obj.category);
        }
};

class config {
    public:
        config( std::string file = "config.xml");
        virtual ~config();
        void set( std::string id, std::string data, std::string category = "");
        std::string get( std::string id, std::string category, std::string defaultValue = "default");

        void save();
        bool load();

        void save_parameter( tinyxml2::XMLDocument *config, tinyxml2::XMLNode *root, std::string name, std::string data);
        std::string load_parameter( tinyxml2::XMLNode *root, std::string name);
    protected:

    private:
        std::vector<config_entity> p_entity;
        std::string p_config_file;
};

#endif // CONFIG_H

#include "config.h"

#include <algorithm>    // std::sort

using namespace tinyxml2;

config::config( std::string file) {
    // load config
    p_config_file = file;
    load();
}

config::~config() {
    save();
}

void config::set( std::string id, std::string data, std::string category) {
    config_entity *l_obj = NULL;

    // search for
    for( int i = 0; i < p_entity.size(); i++) {
        config_entity *l_entity = &p_entity[ i];
        if( l_entity->id == id && category == l_entity->category) {
            l_obj = l_entity;
            break;
        }
    }

    // set data
    if( l_obj ) {
        l_obj->data = data;
    } else {
        l_obj = new config_entity();
        l_obj->data = data;
        l_obj->id = id;
        if( category != "")
            l_obj->category = category;
        p_entity.push_back( *l_obj);
    }
}

std::string config::get( std::string id, std::string category, std::string defaultValue) {
    // search for
    for( int i = 0; i < p_entity.size(); i++) {
        config_entity *l_obj = &p_entity[ i];
        if( l_obj->id == id && l_obj->category == category)
            return l_obj->data;
    }

    // use the default value
    set( id, defaultValue, category);
    return defaultValue;
}

void config::save() {
    XMLDocument l_config;
    std::string l_old_category;

    // create root node
    XMLNode * l_root = l_config.NewElement("config");

    // insert the root node
    l_config.InsertFirstChild(l_root);

    // sort the array
    std::sort( p_entity.begin(), p_entity.end());

    // category
    XMLElement *l_xml_category = NULL;

    for( int i = 0; i < p_entity.size(); i++) {
        config_entity *l_obj = &p_entity[ i];
        // new category
        if( l_obj->category != l_old_category) {
            l_old_category = l_obj->category;

            // Link to the End
            if( l_xml_category)
                l_root->LinkEndChild( l_xml_category);

            // create new node
            l_xml_category = l_config.NewElement( l_old_category.c_str());
        }
        // look if not currupt
        if( l_xml_category) {
            save_parameter( &l_config, l_xml_category, l_obj->id.c_str(), l_obj->data.c_str() );
        }
    }
    // link to the end
    if( l_xml_category)
        l_root->LinkEndChild( l_xml_category);

    // save the file
    l_config.SaveFile( p_config_file.c_str());
}

bool config::load() {
    XMLDocument l_config;

    //load the file
    XMLError l_result = l_config.LoadFile(p_config_file.c_str());
    if( l_result == XML_ERROR_FILE_NOT_FOUND)
        return false;

    // check the file
    XMLCheckResult(l_result);

    // load the root node
    XMLNode * l_root = l_config.FirstChild();
    if (l_root == nullptr)
        return XML_ERROR_FILE_READ_ERROR;

    // get node
    XMLElement* l_xml_category = l_root->FirstChildElement();
    while( l_xml_category != NULL) {
        std::string l_category = l_xml_category->Name();
        // data
        XMLElement* l_xml_id = l_xml_category->FirstChildElement();
        while( l_xml_id != NULL) {
            std::string l_id = l_xml_id->Name();
            std::string l_data = l_xml_id->GetText();

            // set data
            set( l_id, l_data, l_category);

            // next
            l_xml_id = l_xml_id->NextSiblingElement();
        }
        l_xml_category = l_xml_category->NextSiblingElement();
    }

    return true;
}

void config::save_parameter( XMLDocument *config, XMLNode *root, std::string name, std::string data) {
    XMLElement * l_element = config->NewElement( name.c_str());
    l_element->SetText( data.c_str());
    root->LinkEndChild(l_element);
}

std::string config::load_parameter( XMLNode *root, std::string name) {
    XMLElement * l_element = root->FirstChildElement( name.c_str());
    if (l_element == NULL) return "";

    return l_element->GetText();
}

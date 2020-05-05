#ifndef KICAD2SYCIRA_INCLUDED
#define KICAD2SYCIRA_INCLUDED

#include "tinyxml2.h"

class Element;

void parsElements(tinyxml2::XMLDocument &doc, std::vector<Element*> &v_elements);
void parsNets(tinyxml2::XMLDocument &doc, std::vector<Element*> &v_elements);

class Element
{
    public:
        Element(const std::string Name, const std::string Type, const std::string Value ) : elementName_(Name), elementType_(Type), value_(Value)
        {}
        const std::string &GetName()const{return elementName_;}
        const std::string &GetType()const{return elementType_;}
        const std::string &Getvalue()const{return value_;}
        void SetNodelist(std::string nodes) {nodeList_ += std::move(nodes);}
        std::string &GetNodelist(){return nodeList_ ;}

    private:
        std::string elementName_;
        std::string elementType_;
        std::string nodeList_;
        std::string controllingElement_;
        std::string coupledInductors_;
        std::string value_;
        std::string init_value_;


};

class Circuit
{
    public:
        std::string circuitName;
        std::vector<Element> elements;
        const std::string nodeList = "[]"; ///(empty at init)
};




#endif // KICAD2SYCIRA_INCLUDED

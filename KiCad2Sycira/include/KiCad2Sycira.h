#ifndef KICAD2SYCIRA_INCLUDED
#define KICAD2SYCIRA_INCLUDED

#include "tinyxml2.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <sstream>

class Element;

int parsElements(tinyxml2::XMLDocument &doc, std::vector<Element*> &v_elements, std::vector<std::string> &numericValue);
int parsNets(tinyxml2::XMLDocument &doc, std::vector<Element*> &v_elements);
int controllComponentDependencies(const std::vector<Element*> &v_elements);
void numericValues2Maxima(std::vector<std::string> &numericValue);

void write2Maxima( const std::string &maximaTitle, const std::vector<Element*> &v_elements, const std::vector<std::string> &numericValue);

class Element
{
    public:
        Element(const std::string Name, const std::string Type, const std::vector<std::string> Value ) : elementName_(Name), elementType_(Type), value_(Value)
        {}
        const std::string &GetName()const{return elementName_;}
        const std::string &GetType()const{return elementType_;}
        const std::vector<std::string> GetValue()const{return value_;}
        void addToNodeList(std::string node) {nodeList_.push_back(node);}
        std::vector<std::string> GetNodelist(){return nodeList_ ;}

    private:
        std::string elementName_;
        std::string elementType_;
        std::vector<std::string> nodeList_;
        std::string controllingElement_;
        std::string coupledInductors_;
        std::vector<std::string> value_;
        std::string init_value_;

};

#endif // KICAD2SYCIRA_INCLUDED

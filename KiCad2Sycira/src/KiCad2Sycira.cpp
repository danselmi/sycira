#include <iostream>
#include <vector>
#include <algorithm>
#include "KiCad2Sycira.h"

using namespace tinyxml2;
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "no input file name given!\nusage: kicad2sycira <inputnetlist-filename> <outputnetlist-filename>";
        return -1;
    }

    XMLDocument doc;
    doc.LoadFile(argv[1]);
    if(doc.LoadFile(argv[1]) != XML_SUCCESS)
    {
        std::cerr << "failed to open/parse input file!\n";
        return -2;
    }

    std::vector<Element*> v_elements;

    parsElements(doc, v_elements);
    parsNets(doc,v_elements);

    for(auto a: v_elements)
    {
       std::cout << "name  = " << a->GetName()  << std::endl;
       std::cout << "type  = " << a->GetType()  << std::endl;
       std::cout << "value = " << a->Getvalue() << std::endl;
       std::cout << "nodes ="  << a->GetNodelist() <<  std::endl;
    }

    std::cout << "Hello world!" << std::endl;
    return 0;
}

void parsElements(XMLDocument &doc, std::vector<Element*> &v_elements)
{
    std::vector<std::string>bufName;
    std::vector<std::string>bufValue;
    std::vector<std::string>bufType;
    XMLHandle docHandle(&doc);
    XMLElement *levelElement = docHandle.FirstChildElement("export").FirstChildElement("components").ToElement();       ///TODO change to docHandler -> errorhandling
    if(levelElement)
    {
        for (XMLElement* child = levelElement->FirstChildElement("comp"); child != NULL; child = child->NextSiblingElement())
        {
             bufName.push_back(child->Attribute("ref"));
        }

        for (XMLElement* child = levelElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
        {
            XMLElement* ele = child->FirstChildElement("fields")->FirstChildElement("field");
            bufValue.push_back(ele->GetText());
        }

        for (XMLElement* child = levelElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
        {
            XMLElement* ele = child->FirstChildElement("value");
            std::string numericValue(ele->GetText());
            std::cout << "numericValue : " << numericValue << std::endl;
        }

        for (XMLElement* child = levelElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
        {
             bufType.push_back(child->FirstChildElement("libsource")->Attribute("part"));
        }

    }
    else
    {
        std::cout << "no components found" << std::endl;
    }

        for(size_t i = 0; i < bufName.size(); ++i)
        {
          Element *ele = new Element(bufName.at(i), bufType.at(i), bufValue.at(i));
          v_elements.push_back(ele);
        }
}

void parsNets(XMLDocument &doc, std::vector<Element*> &v_elements)
{
    std::cout << "------------------------------" << std::endl;
    XMLHandle docHandle(&doc);
    XMLElement *levelElement = docHandle.FirstChildElement("export").FirstChildElement("nets").ToElement();
    if(levelElement)
    {

        for (XMLElement* child = levelElement->FirstChildElement("net"); child != NULL; child = child->NextSiblingElement())
        {
            for (XMLElement* grandchild = child->FirstChildElement("node"); grandchild != NULL; grandchild = grandchild->NextSiblingElement())
            {
                auto predicate = [&](Element *pElement){return pElement->GetName() == grandchild->Attribute("ref");};
                auto itr = std::find_if(v_elements.begin(), v_elements.end(), predicate);
                if(itr == v_elements.end())
                    std::cout << "net is referring to non existent Component" << std::endl;

                Element *ele = *itr;
                ele->SetNodelist(grandchild->Attribute("pin"));
                std::cout << grandchild->Attribute("ref") << " " << grandchild->Attribute("pin")  << std::endl;
            }
        }

    }
    else
        std::cout << "no nets found" << std::endl;

    std::cout << "------------------------------" << std::endl;

}


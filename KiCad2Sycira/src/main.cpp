#include <iostream>
#include "tinyxml2.h"

using namespace tinyxml2;

int main()
{
    XMLDocument doc;
    doc.LoadFile("kicad_test.xml");
    const char* title;
    XMLElement *levelElement = doc.RootElement()->FirstChildElement("components");
    for (XMLElement* child = levelElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
    {
        XMLElement* ele = child->FirstChildElement("fields")->FirstChildElement("field");
        title = ele->GetText();
        std::cout << title << std::endl;


        // do something with each child element
    }




    std::cout << "Hello world!" << std::endl;
    return 0;
}

#include "KiCad2Sycira.h"

using namespace tinyxml2;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "no input file name given!\nusage: kicad2sycira <inputnetlist-filename> <outputnetlist-filename>";
        return -1;
    }
    std::string title = "Kicad2Sycira";
    XMLDocument doc;
    doc.LoadFile(argv[1]);
    if(doc.LoadFile(argv[1]) != XML_SUCCESS)
    {
        std::cerr << "failed to open/parse input file!\n";
        return -2;
    }

    std::vector<Element*> v_elements;
    std::vector<std::string> numericValue;

    parsElements(doc, v_elements, numericValue);       /// return with returnvalue if fails
    parsNets(doc,v_elements);                          /// ^
    controllComponentDependencies(v_elements);         /// ^
    numericValues2Maxima(numericValue);
    write2Maxima(title, v_elements, numericValue);

    std::cout << "Hello world!" << std::endl;
    return 0;
}

int parsElements(XMLDocument &doc, std::vector<Element*> &v_elements, std::vector<std::string> &numericValue)
{
    XMLHandle docHandle(&doc);
    XMLElement *components = docHandle.FirstChildElement("export").FirstChildElement("components").ToElement();
    if(components)
    {
        for (XMLHandle childHandl = docHandle.FirstChildElement("export").FirstChildElement("components").FirstChildElement("comp"); childHandl.ToElement(); childHandl = childHandl.NextSiblingElement())
        {
            XMLElement *child = childHandl.ToElement();
            std::string Name;
            std::vector<std::string> Value;
            std::string Type;
            if(child)
                Name = child->Attribute("ref");
            for (XMLElement* grandchild = child->FirstChildElement("fields")->FirstChildElement("field"); grandchild != NULL; grandchild = grandchild->NextSiblingElement())
            {
//                XMLElement *child1 = grandchild.FirstChildElement("field").ToElement();  ///TODO why dosnt it work when using child instead of child1?
                if(grandchild)
                {
                    Value.push_back(grandchild->GetText());
                }
            }
            XMLElement *child2 = childHandl.FirstChildElement("libsource").ToElement();
            if(child2)
                Type = child2->Attribute("part");
            std:: string buf =child->FirstChildElement("value")->GetText();
            numericValue.push_back(buf);

            Element *element = new Element(Name, Type, Value);
            v_elements.push_back(element);
        }
    }
    else
    {
        return -3;
        std::cout << "no components found" << std::endl;
    }
    return 0;
}

int parsNets(XMLDocument &doc, std::vector<Element*> &v_elements)
{
    std::set<std::string> netNames;
    XMLHandle docHandle(&doc);
    XMLElement *levelElement = docHandle.FirstChildElement("export").FirstChildElement("nets").ToElement();
    if(levelElement)
    {

        for (XMLElement* child = levelElement->FirstChildElement("net"); child != NULL; child = child->NextSiblingElement())
        {
            std::string key;
            for (XMLElement* grandchild = child->FirstChildElement("node"); grandchild != NULL; grandchild = grandchild->NextSiblingElement())
            {
                auto predicate = [&](Element *pElement){return pElement->GetName() == grandchild->Attribute("ref");};
                auto itr = std::find_if(v_elements.begin(), v_elements.end(), predicate);
                if(itr == v_elements.end())
                    std::cout << "net is referring to non existent Component" << std::endl;

                Element *ele = *itr;

                if( std::string nullString = "0"; child->Attribute("name") == nullString)
                {
                    ele->addToNodeList(nullString);
                    netNames.insert(nullString);
                }

                else if(std::string childBuf = child->Attribute("name"); childBuf.rfind("/",0) == 0)
                {
                    childBuf.erase(0,1);
                    ele->addToNodeList(childBuf);
                    netNames.insert(childBuf);
                }

                else
                {
                    key = child->Attribute("code");
                    std::set<std::string>::iterator it = netNames.find(key);
                    if (it == netNames.end())
                    {
                        ele->addToNodeList(key);
                    }
                    else
                    {
                        while(it != netNames.end())
                        {
                            key = std::to_string(1 + std::stoi(key));
                            it = netNames.find(key);
                        }
                        ele->addToNodeList(key);
                    }
                }
            }
            netNames.insert(key);
        }
    }
    else
    {
       return -4;
        std::cout << "no nets found" << std::endl;
    }


    return 0;

}

int controllComponentDependencies(const std::vector<Element*> &v_elements)
{

     auto predicate = [&](Element *pElement){return pElement->GetType() == "K";};
                auto itr = std::find_if(v_elements.begin(), v_elements.end(), predicate);
                if(itr == v_elements.end())
                    std::cout << "no element Type K found" << std::endl;
    Element *ele = *itr;

    for(size_t i = 0; i < ele->GetValue().size(); ++i)
    {
        if( ele->GetValue().at(i).rfind("K") != 0 )
        {

            auto predicate = [&](Element *pElement){return pElement->GetName() == ele->GetValue().at(i);};
            auto itr = std::find_if(v_elements.begin(), v_elements.end(), predicate);
            if(itr == v_elements.end())
            {
                std::cout << "coupled element does not exist" << std::endl;
                return -5;
            }
            else
                std::cout << "coupled element exists! ele name = " << ele->GetValue().at(i) << std::endl;
        }
    }

    return 0;
}

void numericValues2Maxima(std::vector<std::string> &numericValue)
{
    for(auto a : numericValue)
            std::cout << "numericValue  is : " << a << std::endl;

}


//ckt:["Title",
//[[V0,"V",[1,0],[],[],ue,[]],
// [Ri,"R",[1,2],[],[],Ri,[]],
// [L1,"L",[2,0],[],[],L1,[]],
// [L2,"L",[3,0],[],[],L2,[]],
// [Rl,"R",[3,0],[],[],Rl,[]]],
//[[K1,"K",[],[],[L1,L2],1,[]]],
//[],[]
//];
void write2Maxima( const std::string &maximaTitle, const std::vector<Element*> &v_elements, const std::vector<std::string> &numericValue)
{
    std::cout << "------------------------------" << std::endl;

    std::string quote = "\"";
    std::stringstream maximaString;
    std::stringstream elementString;
    std::stringstream couplingString;
    maximaString << "ckt:[" << maximaTitle << ",[";
    for(size_t i = 0; i < v_elements.size(); ++i)
    {
        if(v_elements.at(i)->GetType() != "K")
        {
            elementString << "[" << v_elements.at(i)->GetName()
                            << quote << v_elements.at(i)->GetType() << quote
                            << "," "[" << v_elements.at(i)->GetNodelist().at(0) << "," << v_elements.at(i)->GetNodelist().at(1)
                            << "]" ",[]," << v_elements.at(i)->GetValue().back() << ",[]],\n";
        }
        else
        {
          couplingString << "[" << v_elements.at(i)->GetName()
                            << quote << v_elements.at(i)->GetType() << quote
                            << ",[],[],[" << v_elements.at(i)->GetValue().at(0) << "," << v_elements.at(i)->GetValue().at(1) << "]" << ",[]],\n";
        }

    }
    maximaString << elementString.str() << couplingString.str() << "[]];";
    std::cout  << maximaString.str() << std::endl;

}

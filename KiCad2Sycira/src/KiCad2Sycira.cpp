#include "KiCad2Sycira.h"

using namespace tinyxml2;

int main(int argc, char *argv[])
{
    if(argc < 3)
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

    std::string outFileName = genBaseFileName(argv[2]);

    std::vector<Element*> v_elements;

    if (parsElements(doc, v_elements))
        return -3;
    if (parsNets(doc, v_elements))
        return -4;
    if (controllComponentDependencies(v_elements))
        return -5;
    createMaximaSession(outFileName + ".wxmx", outFileName + ".mac", write2Maxima(title, v_elements));

    return 0;
}

int parsElements(XMLDocument &doc, std::vector<Element*> &v_elements)
{
    XMLHandle docHandle(&doc);
    XMLElement *components = docHandle.FirstChildElement("export").FirstChildElement("components").ToElement();
    if(!components)
    {
        std::cerr << "no components found\n";
        return -1;
    }


    for (XMLElement *comp = components->FirstChildElement("comp") ; comp ; comp = comp->NextSiblingElement("comp"))
    {
        std::string name = comp->Attribute("ref"); // use refdes as name; assuming it always exists


        std::string type;
        for(XMLElement *libsource = comp->FirstChildElement("libsource") ; libsource ; libsource = libsource->NextSiblingElement("libsource"))
        {
            if(const char *typecstr = libsource->Attribute("part"))
            {
                type = std::string(typecstr, 1);
                break;
            }
        }
        if(type.empty())
        {
            std::cerr << "no type given for element \"" << name << "\"\n";
            return -10;
        }

        XMLElement* fields = comp->FirstChildElement("fields");
        if(!fields)
        {
            std::cerr << "no fields given for element \"" << name << "\"\n";
            return -20;
        }

        std::string value;
        std::string senseElement;
        std::string L1, L2;
        for(XMLElement *field = fields->FirstChildElement("field"); field; field = field->NextSiblingElement())
        {
            if (field->Attribute("name", "SymbolicValue"))
            {
                value = field->GetText();
            }
            if (field->Attribute("name", "SenseElement"))
            {
                senseElement = field->GetText();
            }
            if (field->Attribute("name", "Inductor 1"))
            {
                L1 = field->GetText();
            }
            if (field->Attribute("name", "Inductor 2"))
            {
                L2 = field->GetText();
            }
        }
        if(value.empty())
        {
            std::cerr << "no SymbolicValue given for element \"" << name << "\"\n";
            return -30;
        }
        if(senseElement.empty() && (type == "H" || type == "F"))
        {
            std::cerr << "no senseElement given for element \"" << name << "\"\n";
            return -31;
        }
        if((L1.empty() || L2.empty()) && type == "K")
        {
            std::cerr << "no inductor given for element \"" << name << "\"\n";
            return -32;
        }
        std::string numericValue;
        XMLElement *nval = comp->FirstChildElement("value");
        if(nval)
            numericValue = numericValues2Maxima(nval->GetText());

        if(!senseElement.empty()) // H or F
            v_elements.push_back(new Element(name, type, value, numericValue, senseElement));
        else if(!L1.empty())
            v_elements.push_back(new Element(name, type, value, numericValue, std::array<std::string, 2> {L1, L2}));
        else
            v_elements.push_back(new Element(name, type, value, numericValue));
    }

    return 0;
}

int parsNets(XMLDocument &doc, std::vector<Element*> &v_elements)
{
    XMLHandle docHandle(&doc);
    XMLElement *nets = docHandle.FirstChildElement("export").FirstChildElement("nets").ToElement();
    if(!nets)
    {
        std::cerr << "no nets found" << std::endl;
        return -10;
    }

    for (XMLElement* net = nets->FirstChildElement("net"); net; net = net->NextSiblingElement())
    {
        std::string netName;
        std::string quote = "\"";
        if(std::string nullString = "0"; net->Attribute("name") == nullString)
        {
            netName = nullString;
        }
        else if(std::string childBuf = net->Attribute("name"); childBuf.find("/", 0) == 0)
        {
            childBuf.erase(0,1);
            netName = childBuf;
        }
        else
        {
            netName =  quote + net->Attribute("name") + quote;
        }

        for (XMLElement* node = net->FirstChildElement("node"); node; node = node->NextSiblingElement())
        {
            size_t pinNumber = std::stoi(node->Attribute("pin"));

            auto refIsName = [&](Element *pElement) { return pElement->GetName() == node->Attribute("ref"); };
            auto itr = std::find_if(v_elements.begin(), v_elements.end(), refIsName);
            if(itr == v_elements.end())
            {
                std::cerr << "net is referring to non existent Component!\n";
                return -50;
            }
            (*itr)->addToNodeList(netName, pinNumber-1);
        }
    }

    return 0;
}

int controllComponentDependencies(const std::vector<Element*> &v_elements)
{
    auto isCouplingElement = [](Element *pElement){return pElement->GetType() == "K";};
    auto isCCElement = [](Element *pElement)
    {
        const std::string &type = pElement->GetType();
        return type == "F" || type == "H";
    };

    for (auto ele : v_elements)
    {
        if(isCouplingElement(ele))
        {
            const std::array<std::string, 2> &L = ele->GetCoupledInductors();
            auto isCoupledL1 = [&](const Element *pElement){return pElement->GetName() == L[0];};
            auto isCoupledL2 = [&](const Element *pElement){return pElement->GetName() == L[1];};

            auto itr1 = std::find_if(v_elements.begin(), v_elements.end(), isCoupledL1);
            auto itr2 = std::find_if(v_elements.begin(), v_elements.end(), isCoupledL2);
            if (itr1 == v_elements.end() || itr2 == v_elements.end())
            {
                std::cerr << "elements coupled by element \"" << ele->GetName() << "\" not found!\n";
                return -10;
            }
        }
        else if(isCCElement(ele))
        {
            const std::string &controllingElementName = ele->GetControllingElement();
            auto isControllingElement = [&](Element *pElement){return pElement->GetName() == controllingElementName;};
            auto itr = std::find_if(v_elements.begin(), v_elements.end(), isControllingElement);
            if (itr == v_elements.end())
            {
                std::cerr << "controlling element for element \"" << ele->GetName() << "\" not found!\n";
                return -20;
            }
        }
    }
    return 0;
}

//shameless stolen from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
static inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::string numericValues2Maxima(std::string numValString)
{
    numValString = ltrim(numValString);

    size_t unitPos;
    try
    {
        std::stod(numValString, &unitPos);
    }
    catch (const std::invalid_argument& ia)
    {
        unitPos = 0;
    }
    catch (const std::out_of_range& oor)
    {
        unitPos = 0;
    }

    if (unitPos)
    {
        std::string maximaValueString = "(" + numValString.substr(0, unitPos) + ")";

        std::string unitString = numValString.substr(unitPos);
        unitString = ltrim(unitString);

        std::transform(unitString.begin(), unitString.end(), unitString.begin(),
            [](unsigned char c){ return std::tolower(c); });

        if      (unitString.rfind("mil", 0) == 0) maximaValueString += "*25.4e-6";
        else if (unitString.rfind("meg", 0) == 0) maximaValueString += "*1e6";
        else if (unitString.rfind("f", 0) == 0)   maximaValueString += "*1e-15";
        else if (unitString.rfind("p", 0) == 0)   maximaValueString += "*1e-12";
        else if (unitString.rfind("n", 0) == 0)   maximaValueString += "*1e-9";
        else if (unitString.rfind("u", 0) == 0)   maximaValueString += "*1e-6";
        else if (unitString.rfind("m", 0) == 0)   maximaValueString += "*1e-3";
        else if (unitString.rfind("k", 0) == 0)   maximaValueString += "*1e3";
        else if (unitString.rfind("g", 0) == 0)   maximaValueString += "*1e9";
        else if (unitString.rfind("t", 0) == 0)   maximaValueString += "*1e12";

        return maximaValueString;
    }
    else
        return std::string("");
}

std::string write2Maxima( const std::string &maximaTitle, const std::vector<Element*> &v_elements)
{
    const std::string quote = "\"";
    std::string elementString;
    std::string couplingString;
    std::string numericValueString;
    bool firstElem = true;
    bool firstCoupl = true;
    std::string maximaString = "ckt:[\"" + maximaTitle + "\",\n";
    for(size_t i = 0; i < v_elements.size(); ++i)
    {
        Element *elem = v_elements.at(i);
        if(elem->GetType() != "K")
        {
            if(!firstElem)
                elementString += ",\n";
            elementString += "[" + elem->GetName() + "," + quote + elem->GetType() + quote;
            elementString += ",[" + elem->GetNodelist().at(0) + ", " + elem->GetNodelist().at(1);
            if(elem->GetType() == "E" || elem->GetType() == "G")
                elementString += ", " + elem->GetNodelist().at(2) + ", " + elem->GetNodelist().at(3);
            elementString += "], [";
            if(elem->GetType() == "F" || elem->GetType() == "H")
                elementString += elem->GetControllingElement();
            elementString += "], [], " + elem->GetValue() + ", []]";
            firstElem = false;
        }
        else
        {
            if(!firstCoupl)
                couplingString += ",\n";
            couplingString += "[" + elem->GetName() + "," + quote + elem->GetType() + quote;
            couplingString += ",[],[],[" + elem->GetCoupledInductors().at(0) + ", " + elem->GetCoupledInductors().at(1) + "], " + elem->GetValue() + ", []]";
            firstCoupl = false;
        }
        const std::string &numStr = elem->GetNumericValue();
        if(!numStr.empty())
        {
            if(!numericValueString.empty())
                numericValueString += ", ";
            numericValueString += elem->GetName() + "=" + numStr;
        }

    }
    maximaString += "[" + elementString + "],\n[" + couplingString + "],\n[], [" + numericValueString + "]];";
    std::cout  << maximaString << std::endl;

    return maximaString;
}

std::string genBaseFileName(const std::string &fullfilename)
{
    if(fullfilename.rfind(".mac") == fullfilename.length()-4)
        return fullfilename.substr(0, fullfilename.length()-4);
    if(fullfilename.rfind(".wxmx") == fullfilename.length()-5)
        return fullfilename.substr(0, fullfilename.length()-5);

    return fullfilename;
}


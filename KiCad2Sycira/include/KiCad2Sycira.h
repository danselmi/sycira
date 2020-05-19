#ifndef KICAD2SYCIRA_INCLUDED
#define KICAD2SYCIRA_INCLUDED

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <array>
#include <sstream>
#include <tinyxml2.h>
#include "CreatMaximaSession.h"

class Element;

int parsElements(tinyxml2::XMLDocument &doc, std::vector<Element*> &v_elements);
int parsNets(tinyxml2::XMLDocument &doc, std::vector<Element*> &v_elements);
int controllComponentDependencies(const std::vector<Element*> &v_elements);
std::string genBaseFileName(const std::string &fullfilename);

std::string write2Maxima( const std::string &maximaTitle, const std::vector<Element*> &v_elements);

void numericValues2Maxima(std::vector<Element*> &v_elements);
std::string getSiPrefix(std::string &siPrefix);
std::string removeLetters(std::string &value);

class Element
{
public:
    Element(const std::string &name, const std::string &type, const std::string &value, const std::string &numericValue):
        elementName_(name),
        elementType_(type),
        value_(value),
        numericValue_(numericValue)
    {}
    Element(const std::string &name, const std::string &type, const std::string &value, const std::string &numericValue, const std::string &controllingElement):
        elementName_(name),
        elementType_(type),
        value_(value),
        numericValue_(numericValue),
        controllingElement_(controllingElement)
    {}
    Element(const std::string &name, const std::string &type, const std::string &value, const std::string &numericValue, const std::array<std::string, 2> coupledInductors):
        elementName_(name),
        elementType_(type),
        value_(value),
        numericValue_(numericValue),
        coupledInductors_(coupledInductors)
    {}

    const std::string &GetName()const{return elementName_;}
    const std::string &GetType()const{return elementType_;}
    const std::string &GetValue()const{return value_;}
    const std::string &GetControllingElement()const{return controllingElement_;}
    const std::string &GetNumericValue()const{return numericValue_;}
    void numericToMaxima(std::string numericaValueMaxima) {numericValue_ = numericaValueMaxima;}
    void addToNodeList(std::string node, size_t idx) {nodeList_[idx] = node;}
    const std::array<std::string, 4> &GetNodelist(){return nodeList_ ;}
    const std::array<std::string, 2> &GetCoupledInductors(){return coupledInductors_ ;}

private:
    std::string elementName_;
    std::string elementType_;
    std::string value_;
    std::string numericValue_;
    std::string initValue_;
    std::array<std::string, 4> nodeList_;
    std::string controllingElement_;
    std::array<std::string, 2> coupledInductors_;

};

#endif

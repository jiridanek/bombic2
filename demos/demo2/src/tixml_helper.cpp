
#include <iostream>
#include <string>

#include "tixml_helper.h"
// #include "SDL_lib.h"

using namespace std;

/** @details
 * Vyhledá atribut požadovaného jména,
 * uloží jeho hodnotu typu string.
 * @param El element, v němž se hledá atribut
 * @param name jméno hledaného atributu
 * @param outValue pointer na string, do kterého se uloží nalezená hodnota
 * @return Vrací TIXML_NO_ATTRIBUTE pokud atribut neexistuje, jinak TIXML_SUCCESS.
 */

int QueryStringAttribute(TiXmlElement *El, const char* name,
			std::string* outValue){

	const char* pStr;
	pStr=El->Attribute(name);
	if(!pStr) return TIXML_NO_ATTRIBUTE;
	(*outValue)=pStr;
	return TIXML_SUCCESS;
}

/** @details
 * Najde požadovaný vnořený element a zpracuje jeho argumenty.
 * Při chybě vyvolá výjimku typu string s popisem chyby.
 * @param Element , v němž hledáme vnořený element
 * @param subEl_name název hledaného elementu
 * @param attr_map mapa zpracovaných atributů
 * @see loadItem()
 */

void subElement(TiXmlElement *Element, const char* subEl_name, attr_map_t & attr_map){
	TiXmlElement *subEl= Element->FirstChildElement(subEl_name);
	if(!subEl)
		throw "chybějící element <"+string(subEl_name)+" ...>";
	// atributy
	parseIntAttributes(subEl, attr_map);
/*
	// animace
	int rate;
	string err_rate=("atribut rate v <"+direction2str_(direction)+" ...><animation ...>");

	TiXmlElement *animEl= subEl->FirstChildElement("animation");
	if(!animEl) return; // nemame animaci
	switch(animEl->QueryIntAttribute("rate", &rate)){
		case TIXML_SUCCESS:
			if(rate<=0)
				TiXmlError_(err_rate+" musí mít kladnou hodnotu");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError_(err_rate+" není typu int");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError_(err_rate+" je povinný a chybí");
			break;
	}
	// vsechny slozky animace
	TiXmlElement *animItem= animEl->FirstChildElement("animation_item");
	if(!animItem)
		TiXmlError_("alespoň jeden element <animation_item ...> v <"
				+direction2str_(direction)+" ...> v <animation ...> je povinný");
	while(animItem){
		loadItem_(animItem, direction);
		animItem= animItem->NextSiblingElement("animation_item");
	}
	// ulozit rate
	switch(direction){
		case UP: fps_up_=rate; break;
		case DOWN: fps_down_=rate; break;
		case LEFT: fps_left_=rate; break;
		case RIGHT: fps_right_=rate; break;
		case BURNED: fps_burned_=rate; break;
	}
*/
}

/** @details
 * Zpracuje atributy zadaného elementu
 * a uloží je do mapy číselných atributů.
 * Pokud není atribut typu int, vyvolá výjimku typu string
 * s chybovou hláškou.
 * @param El element, jehož atrobuty se mají zpracovat
 * @param attr_map mapa do níž se uloží hodnoty atributů s názvy jako klíči
 */

void parseIntAttributes(TiXmlElement *El, attr_map_t & attr_map){
	int value;
	// inicializuje prvnim atributem
	TiXmlAttribute *attr= El->FirstAttribute();
	// v kazde iteraci prejdeme na dalsi atribut
	for( ; attr ; attr= attr->Next()){

		switch(attr->QueryIntValue(&value)){
			case TIXML_SUCCESS:
				// vlozit atribut a jeho hodnotu do mapy
				attr_map[attr->Name()]=value;
				break;
			case TIXML_WRONG_TYPE:
				throw string("atribut ")+attr->Name()+" v elementu "
					+El->Value()+" neni typu int";
				break;
		}
	}
}

/** @details
 * Najde hodnotu zadaného atributu, kontroluje, byl-li zadán.
 * @param attr název atributu
 * @param value pointer na integer, do kterého se uloží hodnota,
 *	byla-li zadána
 * @param attr_map mapa atributů
 * @return Vrací true, pokud byl požadovaný atribut nalezen, jinak false.
 */
bool attrIntValue(const char* attr, int & value, const attr_map_t & attr_map ){
	attr_map_t::const_iterator it= attr_map.find(attr);
	if(it==attr_map.end()) return false;
	value = it->second;
	return true;
}

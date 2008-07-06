
#include <iostream>
#include <string>

#include "tixml_helper.h"
// #include "SDL_lib.h"

using namespace std;

/**
 * @details Otevře xml dokument, zkontroluje správnost root elementu,
 * variabilně zkontroluje správnost atributu name proti filename.
 * Pokud kontrola selže, vyvolá TiXmlError().
 * Filename obalí cestou a příponou.
 * @param doc prázdný TiXmlDocument
 * @param filename jméno souboru pro otevření, zadávejte bez cesty a bez přípony
 * @param rootEl_name vyžádané jméno root elementu, pokud je prázdné nekontrokluje se
 * @param checkAttr_name pokud true, provede kontrolu atributu name
 * @return Vrací root element.
 * @see TiXmlError()
 */
TiXmlElement* TiXmlRootElement(TiXmlDocument & doc, std::string & filename,
				const std::string& rootEl_name, bool checkAttr_name){
	// obalit cestou a priponou
	string name("xml/"+filename+".xml");
	// prohodit
	swap(name, filename);

	if(!doc.LoadFile(filename.c_str()))
		TiXmlError(filename,"nelze otevřít XML soubor , nebo není syntakticky správný.");

	TiXmlElement *rootEl;

	if(rootEl_name.empty())
		rootEl = doc.RootElement();
	else { // kontrola root elementu
		rootEl= doc.FirstChildElement(rootEl_name.c_str());
		if(!rootEl)
			TiXmlError(filename,"root element musí být <"+rootEl_name+" ...>.");
	}
	// zkontrolovat atribut name
	if(!checkAttr_name) return rootEl;


	string str;

	switch(QueryStringAttribute(rootEl,"name", &str)){
		case TIXML_SUCCESS:
			if(str!=name)
				TiXmlError(filename,"chybná hodnota atributu name");
			break;
		case TIXML_NO_ATTRIBUTE:
				TiXmlError(filename, "chybějící atribut name");
			break;
	}
	return rootEl;
}


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
	string error;
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

/**
 * @details
 * Naplní parametry mimo prvního hodnotami atributů.
 * Pokud atribut nenajde uloží do parametru -1 (nebo empty string).
 * Pokud je atribut prázdný nebo špatného typu vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param name hodnota stejnojmenného atributu nebo prázdny string
 * @param height hodnota stejnojmenného atributu nebo -1
 * @param width hodnota stejnojmenného atributu nebo -1
 */
void attr_NameHeightWidth(TiXmlElement *El, std::string & name,
			int & height, int & width){
	if(!El){
		name.erase();
		height = width = -1;
		return;
	}

	switch(QueryStringAttribute(El,"name", &name)){
		case TIXML_SUCCESS:
			if(name.empty())
				throw string("atribut name nesmí být prázdný.");
			break;
		case TIXML_NO_ATTRIBUTE:
			name.erase();
			break;
	}

	switch(El->QueryIntAttribute("height", &height)){
		case TIXML_SUCCESS:
			if(height<=0)
				throw string("atribut height musí být větší než 0.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("atribut height není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			height= -1;
			break;
	}

	switch(El->QueryIntAttribute("width", &width)){
		case TIXML_SUCCESS:
			if(width<=0)
				throw string("atribut width musí být větší než 0.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("atribut width není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			width= -1;
			break;
	}
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami adekvátních atributů.
 * Kontroluje také zda je parametr přítomen.
 * Pokud atribut chybí nebo je špatného typu, vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param x hodnota atributu x
 * @param y hodnota atributu y
 */
void attr_XY(TiXmlElement *El, int & x, int & y){
	if(!El){
		x = y = -1;
		return;
	}
	switch(El->QueryIntAttribute("x", &x)){
		case TIXML_SUCCESS:
			if(x<0)
				throw string("atribut x musí být nezáporný.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("atribut x není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				throw string("atribut x chybí.");
			break;
	}

	switch(El->QueryIntAttribute("y", &y)){
		case TIXML_SUCCESS:
			if(y<0)
				throw string("atribut y musí být nezáporný.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("atribut y není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				throw string("atribut y chybí.");
			break;
	}
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami adekvátních atributů.
 * Pokud atribut nenajde, uloží do parametru -1.
 * Pokud atribut chybí nebo je špatného typu, vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param count hodnota atributu count
 */
void attr_Count(TiXmlElement *El, double & count){
	if(!El){
		count = -1;
		return;
	}
	int count_;
	switch(El->QueryIntAttribute("count", &count_)){
		case TIXML_SUCCESS:
			if(count_<1)
				throw string("atribut count musí být kladný.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("atribut count není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				throw string("atribut count chybí.");
			break;
	}
	count= count_;
}

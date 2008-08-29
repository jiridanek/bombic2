
#include <iostream>
#include <string>

#include "tixml_helper.h"

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
		TiXmlError(filename,"can't read XML file or syntactic error occured.");

	TiXmlElement *rootEl;

	if(rootEl_name.empty())
		rootEl = doc.RootElement();
	else { // kontrola root elementu
		rootEl= doc.FirstChildElement(rootEl_name.c_str());
		if(!rootEl)
			TiXmlError(filename,"bad root element, it may be <"+rootEl_name+" ...>.");
	}
	// zkontrolovat atribut name
	if(!checkAttr_name) return rootEl;


	string str;

	switch(QueryStringAttribute(rootEl,"name", &str)){
		case TIXML_SUCCESS:
			if(str!=name)
				TiXmlError(filename,"bad value of attribute name");
			break;
		case TIXML_NO_ATTRIBUTE:
				TiXmlError(filename, "missing attribute name");
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
 * @see parseIntAttributes()
 */
void subElement(TiXmlElement *Element, const char* subEl_name, attr_map_t & attr_map){
	TiXmlElement *subEl= Element->FirstChildElement(subEl_name);
	if(!subEl)
		throw "missing element <"+string(subEl_name)+" ...>";
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
				throw string("attribute ")+attr->Name()+" in element "
					+El->Value()+" must be type of int";
				break;
		}
	}
}

/*
template <class T>
bool stringtox(const string &s, T & val)
{
	stringstream ss(s);
	ss >> val;
	return !ss.fail();
}
*/

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
 * Pokud atribut nenajde uloží do parametru empty string.
 * Pokud je atribut prázdný vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param name hodnota stejnojmenného atributu nebo prázdny string
 */
void attr_Name(TiXmlElement *El, std::string & name){
	if(!El){
		name.erase();
		return;
	}

	switch(QueryStringAttribute(El,"name", &name)){
		case TIXML_SUCCESS:
			if(name.empty())
				throw string("empty value of attribute name.");
			break;
		case TIXML_NO_ATTRIBUTE:
			name.erase();
			break;
	}
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami atributů.
 * Pokud atribut nenajde uloží do parametru -1.
 * Pokud je atribut prázdný nebo špatného typu vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param height hodnota stejnojmenného atributu nebo -1
 * @param width hodnota stejnojmenného atributu nebo -1
 */
void attr_HeightWidth(TiXmlElement *El, int & height, int & width){
	if(!El){
		height = width = -1;
		return;
	}

	switch(El->QueryIntAttribute("height", &height)){
		case TIXML_SUCCESS:
			if(height<=0)
				throw string("value of attribute height must be higher than 0.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("attribute height must be type of int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			height= -1;
			break;
	}

	switch(El->QueryIntAttribute("width", &width)){
		case TIXML_SUCCESS:
			if(width<=0)
				throw string("value of attribute width must be higher than 0.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("attribute width must be type of int.");
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
				throw string("value of attribute x must be higher than 0 or equals.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("attribute x must be type of int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				throw string("missing attribute x.");
			break;
	}

	switch(El->QueryIntAttribute("y", &y)){
		case TIXML_SUCCESS:
			if(y<0)
				throw string("value of attribute y must be higher than 0 or equals.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("attribute y must be type of int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				throw string("missing attribute y.");
			break;
	}
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami adekvátních atributů.
 * Pokud atribut chybí nebo je špatného typu, vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param count hodnota atributu count
 */
void attr_Count(TiXmlElement *El, int & count){
	if(!El){
		count = -1;
		return;
	}
	switch(El->QueryIntAttribute("count", &count)){
		case TIXML_SUCCESS:
			if(count<1)
				throw string("attribute count must be higher than 0.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("attribute count must be type of int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				throw string("missing attribute count.");
			break;
	}
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami adekvátních atributů.
 * Pokud atribut chybí nebo je špatného typu, vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param s hodnota atributu speed
 * @param l hodnota atributu lives
 * @param i hodnota atributu intelligence
 */
void attr_SpeedLivesIntelligence(TiXmlElement *El,
			Uint8 & speed, Uint8 & lives, Uint8 & intelligence){
	if(!El){
		speed = lives = intelligence = 0;
		return;
	}
	int s, l, i;
	switch(El->QueryIntAttribute("speed", &s)){
		case TIXML_SUCCESS:
			if(s<0)
				throw string("attribute speed must be higher than 0.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("attribute speed must be type of int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				throw string("missing attribute speed.");
			break;
	}
	speed = s;
	switch(El->QueryIntAttribute("lives", &l)){
		case TIXML_SUCCESS:
			if(l<1)
				throw string("attribute lives must be higher than 0.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("attribute lives must be type of int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				throw string("missing attribute lives.");
			break;
	}
	lives = l;
	switch(El->QueryIntAttribute("intelligence", &i)){
		case TIXML_SUCCESS:
			if(i<0)
				throw string("attribute intelligence must be higher than 0 or equals.");
			break;
		case TIXML_WRONG_TYPE:
				throw string("attribute intelligence must be type of int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				throw string("missing attribute intelligence.");
			break;
	}
	intelligence = i;
}

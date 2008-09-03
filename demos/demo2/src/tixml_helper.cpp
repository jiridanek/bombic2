
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
 * Vyhledá atribut požadovaného jména,
 * uloží jeho hodnotu typu string.
 * @param El element, v němž se hledá atribut
 * @param name jméno hledaného atributu
 * @param outValue pointer na string, do kterého se uloží nalezená hodnota
 * @return Vrací false pokud element nebo jeho atribut atribut neexistuje, jinak true.
 */
bool readAttr(TiXmlElement *El, const char* name,
			std::string & outValue){
	if(!El) return false;
	const char* pStr;
	pStr=El->Attribute(name);
	if(!pStr) return false;
	outValue= pStr;
	return true;
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
	if(!El || !readAttr(El,"name", name) ){
		name.erase();
		return;
	}

	if(name.empty())
		throw string("empty value of attribute name.");
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
	string s_height, s_width;
	if(readAttr(El,"height", s_height)){
		if(string2x(s_height, height)){
			if(height<=0)
				throw string("value of attribute height must be higher than 0.");
		}
		else	throw string("attribute height must be type of int.");

	}
	else	height= -1;

	if(readAttr(El,"width", s_width)){
		if(string2x(s_width, width)){
			if(width<=0)
				throw string("value of attribute width must be higher than 0.");
		}
		else	throw string("attribute width must be type of int.");


	}
	else	width= -1;
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
	string s_x, s_y;
	if(readAttr(El,"x", s_x)){
		if(string2x(s_x, x)){
			if(x<0)
				throw string("value of attribute x must be higher than 0 or equals.");
		}
		else	throw string("attribute x must be type of int.");
	}
	else	throw string("missing attribute x.");

	if(readAttr(El,"y", s_y)){
		if(string2x(s_y, y)){
			if(y<0)
				throw string("value of attribute y must be higher than 0 or equals.");
		}
		else	throw string("attribute y must be type of int.");
	}
	else	throw string("missing attribute y.");
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
	string s_count;
	if(readAttr(El,"count", s_count)){
		if(string2x(s_count, count)){
			if(count<1)
				throw string("attribute count must be higher than 0.");
		}
		else	throw string("attribute count must be type of int.");
	}
	else	throw string("missing attribute count.");
}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami adekvátních atributů.
 * Pokud atribut chybí nebo je špatného typu, vyvolá výjimku s chybovou hláškou.
 * @param El element jehož hodnoty atributů hledáme
 * @param speed hodnota atributu speed
 * @param lives hodnota atributu lives
 * @param intelligence hodnota atributu intelligence
 */
void attr_SpeedLivesIntelligence(TiXmlElement *El,
			Uint16 & speed, Uint16 & lives, Uint16 & intelligence){
	if(!El){
		speed = lives = intelligence = 0;
		return;
	}
	string s_speed, s_lives, s_intelligence;
	if(readAttr(El,"speed", s_speed)){
		if(string2x(s_speed, speed)){
			if(speed==0)
				throw string("attribute speed must be higher than 0.");
		}
		else	throw string("attribute speed must be type of unsigned int.");
	}
	else	throw string("missing attribute speed.");

	if(readAttr(El,"lives", s_lives)){
		if(string2x(s_lives, lives)){
			if(lives==0)
				throw string("attribute lives must be higher than 0.");
		}
		else	throw string("attribute lives must be type of unsigned int.");
	}
	else	throw string("missing attribute lives.");

	if(readAttr(El,"intelligence", s_intelligence)){
		if(!string2x(s_intelligence, intelligence))
			throw string("attribute intelligence must be type of unsigned int.");
	}
	else	throw string("missing attribute intelligence.");
}

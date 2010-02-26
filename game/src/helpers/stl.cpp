
#include "stl.h"

bool string2x(const std::string & s, std::string & val){
	val = s;
	return true;
}

/** @details
 * Přidá na konec seznamu částí kusy stringu vzniklé rozdělením
 * znakem. Pokud string dělící znak neobsahuje, vloží do seznamu celý string.
 * @param str string, který chceme rozdělit
 * @param delim znak, podle kterého chceme dělit
 * @param parts reference na seznam částí
 */
void explode(const std::string & str, char delim,
		std::vector<std::string> & parts){

	std::stringstream ss(str);
	std::string s;
	while(getline(ss, s, delim)){
		parts.push_back(s);
		if(ss.peek()==' ')
			ss.get();
	}
}

/*
/ ** @details
 * Náhodně, pro všechny prvky spravedlivě
 * vyhodí několik prvních políček z fronty.
 * @param fieldsQueue fronta, ze které chceme vyhazovat
 * /
void popRandomTimes...(fieldsQueue_t & fieldsQueue){
	if(fieldsQueue.empty()){
		return;
	}
	Uint16 randFieldsCount = rand() % fieldsQueue.size();
	for(Uint16 i = 0 ; i < randFieldsCount ; ++i){
		fieldsQueue.pop();
	}
}
*/

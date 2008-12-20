
#include <string>
#include "SDL_lib.h"
#include "tixml_helper.h"
#include "stl_helper.h"
#include "language.h"

Language * Language::myself_ptr_ = 0;

Language* Language::get_instance(){
	if(!myself_ptr_)
		throw std::string("in Language::get_instance(): no Language instance created.");
	return myself_ptr_;
}

/**
 */
Language::Language() {
	if(myself_ptr_)
		throw std::string("in Language constructor: another Language instance created.");
	myself_ptr_ = this;
}

/** @details
 * Vynuluje myself_pointer_.
 */
Language::~Language(){
	myself_ptr_ = 0;
}

void Language::set_language_(const std::string & lang){
	std::string filename(lang);
	rootEl_ = TiXmlRootElement(doc_, filename, LANG_ROOT_ELEMENT, true);
}

const char * Language::get_translation(const char * level1,
			const char * level2, const char * level3) {
	TiXmlElement * el = rootEl_;
	try {
		if(level1)
			el = subElement(el, level1);
		if(level2)
			el = subElement(el, level2);
		if(level3)
			el = subElement(el, level3);
		return el->GetText();
	}
	catch(const std::string & err) {
		TiXmlError(doc_.Value(), err);
	}
	return 0;
}

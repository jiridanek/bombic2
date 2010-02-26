
#include <stdarg.h>
#include <string>
#include <helpers/sdl.h>
#include <helpers/tixml.h>
#include <helpers/stl.h>
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

const char * Language::get_translation(int depth, ...) {
	TiXmlElement * el = rootEl_;
	const char * el_name;
	try {
		va_list ap;
		va_start(ap, depth);
		while(depth--){
			el_name = va_arg(ap, const char *);
			if(el_name && *el_name)
				el = subElement(el, el_name);
		}
		va_end(ap);
		return el->GetText();
	}
	catch(const std::string & err) {
		TiXmlError(doc_.Value(), err);
	}
	return 0;
}

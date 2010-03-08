/** @file singleton.h
 * Makra vhodna pro definici tridy jako singletonu.
 * Singleton je trida, ktera vytvari v programu prave jednu instanci.
 * Konkretne v Bombicovi si tato trida uklada staticky pointer na tuto instanci
 * a statickou funkci get_instance() muzete pointer na tuto instanci ziskat.
 * Pro definovani implementaci teto funkcionality je zde par maker.
 */

#ifndef _SINGLETON_H_GUARD_
#define _SINGLETON_H_GUARD_

#include <stdexcept>
#include <string>

/** Vyjimka pri praci se singletonem. */
class SingletonException: public std::logic_error {
	public:
		SingletonException(const std::string & what_arg):
			std::logic_error(what_arg) {}
};

#define SINGLETON_SHORTCUT(className) className::get_instance()

/** Definice singletonu v tele tridy.
 * Pouzivejte hlavickovem souboru v definici tridy.
 * @param className jmeno tridy (typ)
 */
#define SINGLETON_DEFINITION(className) \
	private: \
		static className * instance_pointer_; \
	public: \
		static inline className * get_instance() { \
			if(!instance_pointer_) { \
				throw SingletonException("no singleton instance created"); \
			} \
			return instance_pointer_; \
		}

#define SINGLETON_INIT(className) \
	className * className::instance_pointer_ = 0

/** Inicializace struktury singletonu.
 * Pouzivejte v kazdem konstruktoru singletonu.
 */
#define SINGLETON_CONSTRUCT \
	do { \
		if(instance_pointer_) { \
			throw SingletonException("another singleton instance created"); \
		} \
		instance_pointer_ = this; \
	} while(0)

/** Zruseni struktury singletonu.
 * Pouzivejte v kazdem destruktoru singletonu.
 */
#define SINGLETON_DESTROY \
	do { \
		instance_pointer_ = 0; \
	} while(0)

#endif

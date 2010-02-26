/** @file menu_base.h
 * Soubor obsahuje třídy,
 * na kterých jsou postaveny jednotlivá menu.
 */

#ifndef MENU_BASE_H
#define MENU_BASE_H

#include <vector>
#include <stack>
#include <helpers/agar.h>

class MenuStack;

#define MENU_ITEM_WIDTH 400
#define MENU_OFFSET 50

class MenuBase {
	public:
		MenuBase();
		virtual ~MenuBase();
		void show();
		void hide();

		static void clearStack();

		static void handlerBack(AG_Event * ev=0);
		static void handlerItems(AG_Event * event);
		static Uint16 activeItem();
		static void handlerIntItem(AG_Event * event);
		static void handlerBoolItem(AG_Event * event);
	protected:
		void createHeading(const char * text);
		AG_Box * createItem(const char * text);
		AG_Box * createItemHoriz(const char * text);
		AG_Checkbox * createCheckboxItem(const char * text, int * value);
		AG_FileDlg * createFileDlg(bool load, const char * path,
				const char * menu, const char * submenu=0);

		AG_Window *win;

		typedef std::vector< AG_Box * > items_t;
		items_t items_;
		static items_t * p_items;

		static MenuStack menu_stack;
};

class MenuStack {
	public:
		void insert(MenuBase * menu);
		void remove( bool quit_on_empty = true);
		void clear();
	private:
		std::stack< MenuBase * > stack_;
};

#endif

#ifndef __STR_LST__
#define __STR_LST__

/*
 * Modified (July 3, 2015 by Nattachart Tamkittikhun) from The Arduino Header ArrayList
 * Written: Obed Isai Rios
 */

#include <Arduino.h>
#include "OtherDef.h"

class StringList {
        private:
		char** stringlist;
		uint16_t size;
	public:	
		StringList(void);
		~StringList(void);
		//void displayStringList();
		void addString(char* item);
		void setString(char* item, int index);
		void removeString(int index);
		void emptyList();
		//void setStringlist(char** stringlist);
		//char** getStringList();
		int getSize();
                String operator[](size_t idx);
};

#endif

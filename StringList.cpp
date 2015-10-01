/*
 * Modified (July 3, 2015 by Nattachart Tamkittikhun) from Arduino Class for ArrayList
 * Written: Obed Isai Rios
 */

#include "StringList.h"

StringList::StringList(){
	this->size = 0;
        //this->stringlist = (char**)malloc((this->size+1)*sizeof(char*));
}

StringList::~StringList(void)
{
        #ifdef DEBUG
        int i;
        for(i=0; i<this->size; i++)
        {
                Serial.println("stringlist: " + String(this->stringlist[i]));
        }
        #endif
        this->emptyList();
}

void StringList::addString(char* item){
	char **neulist = (char**)malloc((this->size+1)*sizeof(char*));
        size_t len;
        #ifdef DEBUG
        Serial.println("sizeof(neulist)"+String(sizeof(neulist)));
        Serial.println("this->size: "+String(this->size));
        #endif
	for(int i=0; i<this->size; i++){
                #ifdef DEBUG
                Serial.println("i, stringlist[i]: "+String(i)+", "+String(stringlist[i]));
                #endif
		neulist[i] = this->stringlist[i];
	}
	
        //Copy all characters.
        #ifdef DEBUG
        Serial.println("item: "+String(item));
        #endif
        len = strlen(item);
        neulist[this->size] = (char*)malloc((len+1)*sizeof(char));
        for(int i=0; i < len; i++)
        {
                neulist[this->size][i] = item[i];
        }
        neulist[this->size][len] = '\0';
        
        #ifdef DEBUG
        if(this->size-1 != 65535)
        {
                Serial.println("neulist[this->size]("+String(this->size)+"): "+String(neulist[this->size]));
                Serial.println("neulist[this->size-1]("+String(this->size-1)+"): "+String(neulist[this->size-1]));
        }
        #endif
	if(this->size > 0)
		free(this->stringlist);
	this->stringlist = neulist;
	this->size = this->size + 1;
}

void StringList::setString(char* item, int index){
	this->stringlist[index] = item;
}

void StringList::removeString(int index){
	char **neulist = (char**)malloc((this->size-1)*sizeof(char*));
	//From Begining
	for(int i=0; i<index; i++){
		neulist[i] = this->stringlist[i]; 
	}
	//From next Index  
	for(int i=index; i<=this->size-1; i++){
		neulist[i] = this->stringlist[i+1];
	}

	free(this->stringlist);
	this->stringlist = neulist;
	this->size = this->size - 1;
}

void StringList::emptyList(){
	size_t i;
        for(i=0; i < this->size; i++)
        {
                free(this->stringlist[i]);
        }
        if(this->size > 0)
                free(this->stringlist);
}

String StringList::operator[](size_t idx)
{
        #ifdef DEBUG
        Serial.println("stringlist[idx]: "+String(this->stringlist[idx]));
        #endif
        return String(this->stringlist[idx]);
}

int StringList::getSize(){
	return this->size;
}


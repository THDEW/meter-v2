#include "MeterUtils.h" 

void MeterUtils::split(String &str, char delimiter, StringList &results)
{
	String tok;
        char *temp;
        size_t i;
        size_t totalLen = str.length();
        temp = NULL;
        
        #ifdef DEBUG
        Serial.println("totalLen: "+String(totalLen));
        #endif
        i = 0;
        while(i < totalLen)
        {
                tok = "";
                while(str[i] != delimiter && i < totalLen)
                {
                        #ifdef DEBUG
                        Serial.println("str[i]: "+String(str[i]));
                        #endif
                        tok += str[i++];
                }
                if(str[i] == delimiter)
                        i++;
                if(temp != NULL)
                        free(temp);
                temp = (char*)malloc((tok.length()+1)*sizeof(char));
                tok.toCharArray(temp, tok.length()+1);
                #ifdef DEBUG
                        Serial.println("tok: "+tok);
                        Serial.println("tok.length(): "+String(tok.length()));
                        Serial.println("temp: "+String(temp));
                #endif
                results.addString(temp);
                #ifdef DEBUG
                        if(results.getSize() > 0)
                                Serial.println("results[i]: "+String(results[results.getSize()-1]));
                #endif
        }
        #ifdef DEBUG
                for(i=0; i<results.getSize(); i++)
                {
                          Serial.println("results.getSize(): "+String(results[i]));
                }
        #endif
}

char MeterUtils::getLine(File &f, String &line)
{
	char c;
        int i = 0;
	line = String();
        #ifdef DEBUG
        Serial.println("getLine: ");
	#endif
        do
	{
                c = f.read();
                #ifdef DEBUG
                Serial.println("line00: "+ String(c)+","+String(i));
                Serial.println("line01: "+ String((int)c)+","+String(i));
                #endif
                if(c != '\n' && c != -1 && c != F_LAST_CHAR)
                {
                    #ifdef DEBUG
                    Serial.println("line1: "+ String(c)+","+String(i));
                    #endif
                    if(c != ' ')
                    {
                            #ifdef DEBUG
                            Serial.println("line2: "+ String((int)c)+","+String(i));
                            #endif
    		            line += c;
                    }
                    i++;
                }
	}while(c != '\n' && c != -1 && c != F_LAST_CHAR);
        return c;
}

void MeterUtils::enableEthernet()
{
	//Assume all Ethernet's, SD's, VTCT's slave select pins have been set to be output.
	ETH_SS_DDR |= (1 << ETH_SS);
        ETH_SS_PORT |= (1 << ETH_SS); //set bit (enable)
	SD_SS_DDR &= ~(1 << SD_SS);
	VTCT_SS_DDR &= ~(1 << VTCT_SS);
}
void MeterUtils::enableSDCard()
{
	//Assume all Ethernet's, SD's, VTCT's slave select pins have been set to be output.
	ETH_SS_DDR &= ~(1 << ETH_SS);
        SD_SS_DDR |= (1 << SD_SS);
	SD_SS_PORT |= (1 << SD_SS); //set bit (enable)
	VTCT_SS_DDR &= ~(1 << VTCT_SS);
}
void MeterUtils::enableVTCT()
{
	//Assume all Ethernet's, SD's, VTCT's slave select pins have been set to be output.
	ETH_SS_DDR &= ~(1 << ETH_SS); //Ethernet slave select pin to be input
	SD_SS_DDR &= ~(1 << SD_SS);
        VTCT_SS_DDR |= (1 << VTCT_SS);
	VTCT_SS_PORT &= ~(1 << VTCT_SS); //clear bit (enable)
}
void MeterUtils::disableSDCard()
{
        SD_SS_PORT &= ~(1 << SD_SS); //clear bit (disable SD card)
}

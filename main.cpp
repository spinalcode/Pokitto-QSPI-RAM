
/*
This code uses software QSPI and manages 45FPS
*/

int sx=0;
int sy=0;

#include <Pokitto.h>

#include "globals.h"
#include "font.h"
#include "buttonhandling.h"
#include "ram.i"
#include "screen.h"

#include "plasma.h"

// print text
void myPrint(char x, char y, const char* text) {
  uint8_t numChars = strlen(text);
  uint8_t x1 = 0;//2+x+28*y;
  for (uint8_t t = 0; t < numChars; t++) {
    uint8_t character = text[t] - 32;
    Pokitto::Display::drawSprite(x+((x1++)*8), y, font88[character]);
  }
}


char tempText[64];

DigitalOut TEMP_CS(P1_19);

int main(){
    using PC=Pokitto::Core;
    using PD=Pokitto::Display;
    using PB=Pokitto::Buttons;
    using PS=Pokitto::Sound;

    PC::begin();
    PD::invisiblecolor = 0;
    PD::adjustCharStep = 0;
    PD::adjustLineStep = 0;

    PD::lineFillers[0] = myBGFiller; // A custom filler to draw from SRAM HAT to screen
    TEMP_CS = 1;

    initRAM();

    TEMP_CS = 0;

    // from this point, use qspi to communicate with the ram chip

    //uint8_t tempData[2];
    //readFromAddressQuad(0, &tempData[0], 1);

    // load a loarger than screen image to RAM, this one takes up nearly all of it.
//    writeToAddressQuad(0, &background1[0], 320*204);

//    Pokitto::Display::load565Palette(background1_pal); // load a palette the same way as any other palette in any other screen mode

    uint8_t tempData[]={255,255};
    uint8_t tempData2[]={0,0};

    writeToAddressQuad(240, tempData, sizeof(tempData));
    TEMP_CS = 1;
    TEMP_CS = 0;
    readFromAddressQuad(240, tempData2, sizeof(tempData2));
    TEMP_CS = 1;


    clearQuad();
    make_pal();
    make_plasma();

    int palOff=0;
    while( PC::isRunning() ){
        
        updateButtons();

        if(_Left[HELD] && sx>0) sx--;
        if(_Right[HELD] && sx<100) sx++;
        if(_Up[HELD] && sy>0) sy--;
        if(_Down[HELD] && sy<28) sy++;
        
        if(!PC::update()) continue;

        int num = pal[0];
        for(int t=0; t<255; t++){
            pal[t] = pal[t+1];
        }
        pal[255]=num;
        Pokitto::Display::load565Palette(&pal[0]); // load a palette the same way as any other palette in any other screen mode

        sprintf(tempText,"FPS:%d",fpsCount);
        myPrint(0,0,tempText);
/*
        myPrint(0,8,"Written  Read    ");

        for(int t=0; t<8; t++){
            int c,k;
            tempText[0]=0;
            for(c = 7; c >= 0; c--){
                k = tempData[t] >> c;
                if (k & 1){
                    strcat(tempText, "1");
                }else{
                    strcat(tempText, "0");
                }
            }
            strcat(tempText, 0);
            myPrint(0,16+t*8,tempText);
        }

        for(int t=0; t<8; t++){
            int c,k;
            tempText[0]=0;
            for(c = 7; c >= 0; c--){
                k = tempData2[t] >> c;
                if (k & 1){
                    strcat(tempText, "1");
                }else{
                    strcat(tempText, "0");
                }
            }
            strcat(tempText, 0);
            myPrint(72,16+t*8,tempText);
        }
*/

        fpsCounter++;
        frameCount++;

        if(PC::getTime() >= lastMillis+1000){
            lastMillis = PC::getTime();
            fpsCount = fpsCounter;
            fpsCounter = 0;
        }

    }
    
    return 0;
}

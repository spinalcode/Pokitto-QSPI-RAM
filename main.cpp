
/*
This code uses software QSPI and manages 45FPS
*/

#define screenWidth 400

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

    initRAM();

    // from this point, use qspi to communicate with the ram chip

    // load a larger than screen image to RAM, this one takes up nearly all of it.
    writeToAddressQuad(0, &background1[0], 400*300);
    Pokitto::Display::load565Palette(background1_pal); // load a palette the same way as any other palette in any other screen mode


    int palOff=0;
    bool doPlasma = false;
    while( PC::isRunning() ){
        
        if(!PC::update()) continue;
        updateButtons();

        if(_Left[HELD] && sx>0) sx--;
        if(_Right[HELD] && sx<180) sx++;
        if(_Up[HELD] && sy>0) sy--;
        if(_Down[HELD] && sy<124) sy++;
        if(_B[NEW]){
            doPlasma = true;
            clearQuad();
            make_pal();
            make_plasma();
        }
        
        if(doPlasma){
            int num = pal[0];
            for(int t=0; t<255; t++){
                pal[t] = pal[t+1];
            }
            pal[255]=num;
            Pokitto::Display::load565Palette(&pal[0]); // load a palette the same way as any other palette in any other screen mode
        }
        
        sprintf(tempText,"FPS:%d",fpsCount);
        myPrint(0,0,tempText);

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

/*******************
** Gabriele Di Bari
** 2011/06/15
********************/

#include "window.h"
#include "render.h"
#include "chip.h"
#include <time.h>
#include <windows.h>

unsigned short KeyPress(int i){
    switch(i){
            case 0x0:   return char_key['0'];
            case 0x1:   return char_key['1'];
            case 0x2:   return char_key['2'];
            case 0x3:   return char_key['3'];
            case 0x4:   return char_key['4'];
            case 0x5:   return char_key['5'];
            case 0x6:   return char_key['6'];
            case 0x7:   return char_key['7'];
            case 0x8:   return char_key['8'];
            case 0x9:   return char_key['9'];
            case 0xA:   return char_key['A'];
            case 0xB:   return char_key['B'];
            case 0xC:   return char_key['C'];
            case 0xD:   return char_key['D'];
            case 0xE:   return char_key['E'];
            case 0xF:   return char_key['F'];
            default:    return 0;
    }
}
unsigned short Wait(){
    for(;;){
            if(char_key['0']) return 0x0;
            if(char_key['1']) return 0x1;
            if(char_key['2']) return 0x2;
            if(char_key['3']) return 0x3;
            if(char_key['4']) return 0x4;
            if(char_key['5']) return 0x5;
            if(char_key['6']) return 0x6;
            if(char_key['7']) return 0x7;
            if(char_key['8']) return 0x8;
            if(char_key['9']) return 0x9;
            if(char_key['A']) return 0xA;
            if(char_key['B']) return 0xB;
            if(char_key['C']) return 0xC;
            if(char_key['D']) return 0xD;
            if(char_key['E']) return 0xE;
            if(char_key['F']) return 0xF;
        UpDataWindow();
    }
}

int main(int argc, const char* argv[])
{
        //////////////////////////////////////////

    float scalaX=10.0f,scalaY=10.0f;
    StartWindow(64*scalaX,32*scalaY);
    //manager event
    Event event;
    event.KeyPress=KeyPress;
    event.Wait=Wait;
    //alloc chip
    Chip *_chip_8;
    Chip_8(&_chip_8);
    _chip_8->input=&event; //aggiungi eventi

    //set rom ROM
    Rom *rom=NULL;
    //input file
    if(argc<2){
        #ifdef _WIN32
        	OPENFILENAME ofn;
            char szFileName[MAX_PATH] = "";

            ZeroMemory(&ofn, sizeof(ofn));

            ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
            ofn.hwndOwner = NULL;//*this;
            ofn.lpstrFilter = "All format\0*.*\0";
            ofn.lpstrTitle = TEXT("Open File");
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT;
            ofn.lpstrDefExt = "obj";

            if(GetOpenFileName(&ofn)){
               //load Rom
               rom=_chip_8->MakeRom( ofn.lpstrFile );
            }
        #else
            printf("drop on exe rom");
            return 0;
        #endif
        }
        else{
            //load Rom from console
            rom=_chip_8->MakeRom(argv[1]);
        }

    _chip_8->SetRom(_chip_8,rom);
    //clean screen (openGL)
    Cls(0,0,0);
    while(!closeWindow){
        Sleep(5);//....chip 8 is a slow cpu
        Scale(scalaX,scalaY);
        _chip_8->Execute(_chip_8);
        UpDataWindow();
    }

    //dealloc rom
    free(rom);
	//dealloc chip
    _chip_8->Free(_chip_8);
	//close window
    CloseTheWindow();
    return 0;
}

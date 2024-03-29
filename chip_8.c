/*******************
** Gabriele Di Bari
** write 
**	2011/06/15
** translation comments
**   2013/03/23
********************/
#include "chip.h"
#include "render.h"
#include "window.h"
#include <stdio.h>
#include <time.h>

#define byte_8 unsigned char
#define byte_16 unsigned short
#ifdef _DEBUG_
    #define _DEBUG(x) printf(x);
    #define _DEBUG2(x,y) printf(x,y);
    #define _DEBUG3(x,y,z) printf(x,y,z);
#else
    #define _DEBUG(x)
    #define _DEBUG2(x,y)
    #define _DEBUG3(x,y,z)
#endif

//list fonts
static byte_8 sprite_font[16*8]={
        0xF0, 0x90, 0x90, 0x90, 0xF0,// 0
		0x20, 0x60, 0x20, 0x20, 0x70,// 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0,// 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0,// 3
		0x90, 0x90, 0xF0, 0x10, 0x10,// 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0,// 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0,// 6
		0xF0, 0x10, 0x20, 0x40, 0x40,// 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0,// 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0,// 9
		0xF0, 0x90, 0xF0, 0x90, 0x90,// A
		0xE0, 0x90, 0xE0, 0x90, 0xE0,// B
		0xF0, 0x80, 0x80, 0x80, 0xF0,// C
		0xE0, 0x90, 0x90, 0x90, 0xE0,// D
		0xF0, 0x80, 0xF0, 0x80, 0xF0,// E
		0xF0, 0x80, 0xF0, 0x80, 0x80 // F
};
//memory struct
struct _Memory{
    byte_16 pc;              //program counter
    byte_16 mar;             //mar (memory address register)
    byte_8  reg[16];         //registri (15)+"carry-flag" (VF) //1 bit
    byte_8  sc;              //stack counter
    byte_16 stack[16];       //stack    (16)
    byte_8 tSound,tDelay;    //timer-program e timer-audio
    byte_8 *program_memory;  //pointer to rom
    byte_8 screen[64*32];    //screen size
    byte_8 old_screen[64*32];//screen last (xor mode)
};

//rom memory struct
struct _Rom{
    //4095 8-bit fields
	//first 512 unused
    byte_8 list[4095];
};

//inizialization memory
void _ZeroCpu(struct _Chip *chip){
    //inizialization memory
    //first 512 unused (chip8 boot loader)
    //0x200 = 512
    chip->memory->pc=0x200;
    //mar set to 0
    chip->memory->mar=0;

    //index for
    byte_16 i;
    //inizialization registers
    for(i=0;i<16;i++)
      chip->memory->reg[i]=0;
    //inizialization stack
    for(i=0;i<16;i++)
      chip->memory->stack[i]=0;
    //inizialization id stak
    chip->memory->sc=0;

    //timers to 0
    chip->memory->tDelay=0;
    chip->memory->tSound=0;

    //inizialization screen
    for(i=0;i<64*32;i++)  chip->memory->screen[i]=0;
}
//imposta room
void _SetRom(struct _Chip *chip,Rom *rom){
    //inizialization memory
    _ZeroCpu(chip);
    //buffer containing those OP codes 
    chip->memory->program_memory=rom->list;
    //setup fonts
    int i;
    for(i = 0; i < 64*8; i++)
    chip->memory->program_memory[i]=sprite_font[i];

}
//load rom from file
Rom* _MakeRom(const char* file_src){
    //alloc rom memory
    Rom *out=malloc(sizeof(struct _Rom));
	//open file
	FILE * file = fopen(file_src, "rb");
    #ifdef _DEBUG_
    if(file==0)
        _DEBUG("load rom: can't read file\n")
    #endif
    //get size file
    fseek(file, 0L, SEEK_END);
    long size_buffer = ftell(file);
    fseek(file, 0L, SEEK_SET);
    //debug
    _DEBUG2("load rom: size_buffer=%d\n",size_buffer)
    //alloca la memoria
	char * buffer_read = (char*)malloc(sizeof(char) * size_buffer);
    #ifdef _DEBUG_
    if(buffer_read==0)
        _DEBUG("load rom: error memory allocation\n")
    #endif
    //read buffer
	fread (buffer_read, 1, size_buffer, file);

    #ifdef _DEBUG_
    if((size_buffer-512)>4096)
        _DEBUG("load rom:rom too big\n")
    #endif
	//copy rom into buffer
    int i = 0;
    for(i = 0; i < size_buffer; ++i){
         out->list[i+512]=(byte_8)(buffer_read[i]);
    }
    #ifdef _LOG_OUT
    _DEBUG("load rom:\n PC   BC \n")
    for(i = 0; i < size_buffer; i+=2)
     _DEBUG3("%4.0X %4.0X\n",i+512,((out->list[i+512]<<8)+out->list[i+513])&0xFFFF)
    #endif
    //close file
    close(file);
    //clean
    free(buffer_read);
    //return rom
    return out;
}
//esegue una lista comandi Rom
int _Execute(struct _Chip *chip){
    //set random values
    srand(time(NULL));
    // the program counter increases 
    #define plus_pc  chip->memory->pc+=2
    #define plus_plus_pc  chip->memory->pc+=4
    //id register carry-flag 
    #define VF_REG   15
    //max size screen
    #define MAX_SCREEN   (64*32)
    //16 bits per instruction
    //but it are stored in 8 bits
    //so  "field1"+"field2" 
    byte_16 line_code_read=((chip->memory->program_memory[chip->memory->pc]<<8)+chip->memory->program_memory[chip->memory->pc+1]);

    //sprite data
    byte_8 sprite_x,sprite_y;          //coordinates
    byte_8 sprite_lines,sprite_data;   //righe sprite
    byte_8 screen_x,screen_y;          //coordinates on screen
    #define S_MAX_X 64
    #define S_MAX_Y 32

    //temporary variables
    byte_16 i;
    //registers x,y;
    byte_8 reg_x,reg_y;

    switch(line_code_read & 0xF000){ //code mask to select type opcode

        //0nnn
        case 0x0000:
            switch(line_code_read & 0x00FF){
                case 0x00E0: //CLS 
					//screen cleaner
                    for(i=0;i<(64*32);i++)  chip->memory->screen[i]=0;
                    //next
                    plus_pc;
                break;
                case 0x00EE: //RET 
					//pop stack
                    chip->memory->sc--; 
					//jump to line program saved on the stack
                    chip->memory->pc= chip->memory->stack[chip->memory->sc];
                    //next
                    plus_pc;
                break;
                ///////////////////
                //  0nnn - SYS addr 
				//  jump to NNN line program  (old)
                default:
                        plus_pc;
                    //chip->memory->pc=(line_code_read&0x0FFF);

                break;
            }
        break;
        //  1nnn - JP addr //jump to NNN line program
        case 0x1000:
               chip->memory->pc=(line_code_read&0x0FFF);
        break;
        //  2nnn - CALL addr  //call a subrutine
        case 0x2000:
			   //push pc in stack
               chip->memory->stack[chip->memory->sc]=chip->memory->pc;
               chip->memory->sc++; 
			   //jump to NNN line program			   
               chip->memory->pc=(line_code_read&0x0FFF); 
        break;
        ///////////////////
        //IF (3XKK) (if X (register id) == KK then jump)
        case 0x3000:
            if(chip->memory->reg[((line_code_read&0x0F00)>>8)]==(line_code_read&0x00FF))
               plus_plus_pc;
            else
                plus_pc;
        break;
        //IF (4XKK) (if X (register id) != KK then jump)
        case 0x4000:
            if(chip->memory->reg[((line_code_read&0x0F00)>>8)]!=(line_code_read&0x00FF))
               plus_plus_pc;
            else
                plus_pc;
        break;
        //IF (5XY0)(if X (register id) == Y (register id) then jump)
        case 0x5000:
            if(chip->memory->reg[((line_code_read&0x0F00)>>8)]==chip->memory->reg[(line_code_read&0x00F0>>4)])
               plus_plus_pc;
            else
                plus_pc;
        break;
        //LD, 0x6XNN, save NN (8bits) in X register
        case 0x6000:
            chip->memory->reg[((line_code_read&0x0F00)>>8)]=(line_code_read&0x00FF); //cast 16->8 implicito (no VS)
            //next
            plus_pc;
        break;

        //LD, 0x7XNN, adding  NN (8 bits) into X register
        case 0x7000:
            chip->memory->reg[((line_code_read&0x0F00)>>8)]+=(line_code_read&0x00FF); //cast 16->8 implicito (no VS)
            //next
            plus_pc;
        break;

        //OPERATION BETWEEN REGISTERS:
        case 0x8000:
            reg_x=((line_code_read&0x0F00)>>8);
            reg_y=((line_code_read&0x00F0)>>4);
            switch(line_code_read & 0x000F){
                //LD Y->X, 0x8XY0, copy  Y register into X register
                case 0x0:  chip->memory->reg[reg_x]=chip->memory->reg[reg_y];  plus_pc; break;
                // Y | X, 0x8XY1, OR between Y and X
                case 0x1: chip->memory->reg[reg_x]|=chip->memory->reg[reg_y];  plus_pc; break;
                // Y & X, 0x8XY2, AND between Y and X
                case 0x2: chip->memory->reg[reg_x]&=chip->memory->reg[reg_y];  plus_pc; break;
                // Y ^ X, 0x8XY3, XOR between Y and X
                case 0x3: chip->memory->reg[reg_x]^=chip->memory->reg[reg_y];  plus_pc; break;
                // Y ADD X, 0x8XY4, adding Y register into X register
                case 0x4:
                    //overflow on VF
                    if ((chip->memory->reg[reg_y]+chip->memory->reg[reg_x]) > 0xFF)
                     chip->memory->reg[VF_REG]=1;
                    else
                     chip->memory->reg[VF_REG]=0;
                    //adding....
                    chip->memory->reg[reg_x]+=chip->memory->reg[reg_y];
                    //next
                    plus_pc;
                break;
                // Y MINUS X, 0x8XY5, subtraction il registro Y register from X register
                case 0x5:
                    //overflow on VF
                    if (chip->memory->reg[reg_y]>(chip->memory->reg[reg_x]))
                    chip->memory->reg[VF_REG]=0x00;
                    else
                    chip->memory->reg[VF_REG]=0x01;
                    //subtraction
                    chip->memory->reg[reg_x]-=chip->memory->reg[reg_y];
                    plus_pc;
                break;
                // X SHR 1 0x8XY6, right shift X register, last bit into VF
                case 0x6:
                    //overflow on VF
                    chip->memory->reg[VF_REG]=chip->memory->reg[reg_x]&0x01;
                    //Right shift
                    chip->memory->reg[reg_x]>>=1;
                    //next
                    plus_pc;
                break;
                // Y MINUS X, 0x8XY5, subtraction il registro Y register from X register (inverse) x=y-x
                case 0x7:
                    //overflow on VF
                    if (chip->memory->reg[reg_y]<(chip->memory->reg[reg_x]))
                    chip->memory->reg[VF_REG]=0x00;
                    else
                    chip->memory->reg[VF_REG]=0x01;
                    //subtraction
                    chip->memory->reg[reg_x]=chip->memory->reg[reg_y]-chip->memory->reg[reg_x];
                    plus_pc;
                break;
                // X SHR 1 0x8XYE, left shift X register, last bit into VF
                case 0xE:
                    //overflow on VF
                    chip->memory->reg[VF_REG]=(chip->memory->reg[reg_x]&0x80)>>7; // 1000 0000
                    //left shift
                    chip->memory->reg[reg_x]<<=1;
                    //next
                    plus_pc;
                break;
            }
        break;
        //////////////////////////////////(other IF)
        //IF (9XY0) (if X (register id) != Y (register id)  then jump)
        case 0x9000:
            if(chip->memory->reg[((line_code_read&0x0F00)>>8)]!=chip->memory->reg[((line_code_read&0x00F0)>>4)])
               plus_plus_pc;
            else
                plus_pc;
        break;
        //////////////////////////////////
        // MAR OPERATION (i)
        //(ANNN) set mar (i) = NNN
          case 0xA000:
          chip->memory->mar=line_code_read&0x0FFF;
          plus_pc;
          break;
        //////////////////////////////////

        //(BNNN) JP V0, jump to NNN+V0   (V0-->register[0])
          case 0xB000:

          chip->memory->pc=(line_code_read&0x0FFF)+chip->memory->reg[0x0];

          break;
        //(CXKK) RND - get a random (KK) value (between 0 and 255) and save into X register
          case 0xC000:
          chip->memory->reg[(line_code_read&0x0F00)>>8]= (rand()&(line_code_read&0x00FF));
          plus_pc;
          break;

        //////////////////////////////////
        // DRAW SPRITE
        // (DXYN) draw sprite into buffer
        // get coordinates from register[X] and register[Y],
        // read the span from MAR(I), for n bits.
        // if detected a collision set KF to 1 (xor mode)
           case 0xD000:
                //get coordinates x,y
                sprite_x=chip->memory->reg[(line_code_read&0x0F00)>>8];  //X cord
                sprite_y=chip->memory->reg[(line_code_read&0x00F0)>>4];  //Y cord
                sprite_lines=(line_code_read&0x000F);                    //line N-bits
                //collision set to 0
                chip->memory->reg[VF_REG] = 0x00;
                //mi muovo per il numero di righe indicate
                for(screen_y=0; screen_y<sprite_lines; screen_y++){
					//get span (n bits) from MAR
                    sprite_data=chip->memory->program_memory[chip->memory->mar+screen_y]; 
                    //copy span into screen buffer and detect the collision (XOR)
                    //max size is 8 pixel (8 bit),
                    for(screen_x=0;screen_x < 8 ;screen_x++){
                        //check if isn't a negative value 8 bit (1000 000>=)
                        //check if out of screen
                        if((sprite_data&(0x80>>screen_x))!=0 &&(screen_x+ sprite_x +((screen_y+sprite_y)*S_MAX_X))< MAX_SCREEN){
                        //write bit (in xor mode)
                        chip->memory->screen[screen_x+ sprite_x +((screen_y+sprite_y)*S_MAX_X)] ^=1;
                        //if it is a black pixel, after xor, we found a collision
                        if(!chip->memory->screen[screen_x+ sprite_x +((screen_y+sprite_y)*S_MAX_X)])
                            chip->memory->reg[VF_REG] = 0x01;

                       }
                    }
                }
                chip->Draw(chip);
                plus_pc;
           break;
        //
        //////////////////////////////////
        // Input
        // input from keyboard
        // key press EXkk
        case 0xE000:
            switch(line_code_read & 0x00FF){
                //if press X (register id) 
                case 0x009E:
                  if(chip->input->KeyPress(chip->memory->reg[(line_code_read&0x0F00)>>8]))
                  plus_plus_pc;
                    else
                  plus_pc;
                break;
				//if released X (register id) 
                case 0x00A1:
                  if(chip->input->KeyPress(chip->memory->reg[(line_code_read&0x0F00)>>8])==0)
                  plus_plus_pc;
                    else
                  plus_pc;
                break;

            }
        break;
        //i/o FILE and FX commands
        case 0xF000:
			//commands (io) // 0x00CC
            switch(line_code_read & 0x00FF){
                //save into register[X] the delay timer X<-tD
                case 0x0007: //CXCC
                  chip->memory->reg[(line_code_read&0x0F00)>>8]=chip->memory->tDelay; plus_pc;
                break;
                //wait press a key and save it into register[x]   X<-KEY
                case 0x000A: //CXCC
                  chip->memory->reg[(line_code_read&0x0F00)>>8]=chip->input->Wait(); plus_pc;
                break;
                //set delay timer from X register, tD<-X
                case 0x0015: //CXCC
                  chip->memory->tDelay=chip->memory->reg[(line_code_read&0x0F00)>>8]; plus_pc;
                break;
                //set sound timer from X register, tS<-X
                case 0x0018: //CXCC
                  chip->memory->tSound=chip->memory->reg[(line_code_read&0x0F00)>>8]; plus_pc;
                break; //CXCC
                //mar operation, mar=mar+reg[x] (mar(i))
                case 0x001E://CXCC
                //somma
                  chip->memory->mar+=chip->memory->reg[(line_code_read&0x0F00)>>8];
                  plus_pc;
                break;
                //I is set the address for the hexadecimal character sprite referred to by the register 5 bytes high
                case 0x0029://CXCC 
                    chip->memory->mar=chip->memory->reg[(line_code_read&0x0F00)>>8]*0x5;
                    plus_pc;
                break;
                //I is set the address for the hexadecimal character sprite referred to by the register 10 bytes high (super)
                case 0x0030://CXCC
                    chip->memory->mar=chip->memory->reg[(line_code_read&0x0F00)>>8]*0x10;
                    plus_pc;
                break;
				// take value from register[x] [0 to 255] an convert it in BCD --> [ example: 255 into 2 , 5 , 5 ]
				// save values into program memory [ MAR, MAR+1,MAR+2 ]
                case 0x0033://CXCC 
                    chip->memory->program_memory[chip->memory->mar]=chip->memory->reg[(line_code_read&0x0F00)>>8]/100;
                    chip->memory->program_memory[chip->memory->mar+1]=(chip->memory->reg[(line_code_read&0x0F00)>>8]/10)%10;
                    chip->memory->program_memory[chip->memory->mar+2]=(chip->memory->reg[(line_code_read&0x0F00)>>8]%100)%10;
                    plus_pc;
                break;
                //////////////////////////////////////////////////////////////////////////////////////////////////
				// save/copy 0..to..X registers into program memory [ register[0..X]-->MAR (i...i+x), MAR<-MAR+X+1
                case 0x0055:
                    //copy from registers
                    for(i=0;i<=((line_code_read&0x0F00)>>8);i++)
                    chip->memory->program_memory[chip->memory->mar+i]=chip->memory->reg[i];
                    plus_pc;
                break;
                ///////////////////////////////////////////////////////////////////////////////////////////////////
                // loat 0..to..X registers from program memory [ register[0..X]<-- MAR (i...i+x), MAR<-MAR+X+1 ) ]
                case 0x0065:
                    //copy into registers
                    for(i=0;i<=((line_code_read&0x0F00)>>8);i++)
                    chip->memory->reg[i]=chip->memory->program_memory[chip->memory->mar+i];
                    plus_pc;
                break;
				//errors
                default:
                    _DEBUG("NOT SUPORT ASM COMAND (FX)")
                break;
            }
        break;

        //errors
        default:
            _DEBUG("NOT SUPORT ASM COMAND")
            return -1;
           break;
    }

        //update timer
        if(chip->memory->tDelay>0)  chip->memory->tDelay--;
        //update sound timer
        if(chip->memory->tSound>0)  chip->memory->tSound--;

    //----------------
    return 0; //------
    //----------------
};
//draw function
void _Draw(struct _Chip *chip){
    int x,y;
     for(x=0;x<64;x++){
        for(y=0;y<32;y++){
         if(chip->memory->screen[x+(y*64)]>0){
			//set color
            Color(255,255,255);
			//draw
            DrawPoint(x,y);
         }
         //cleaning  (XOR MODE)
         else if(chip->memory->old_screen[x+(y*64)]==0){
			//set color
             Color(0,0,0);
			//draw
             DrawPoint(x,y);
            }
        }
    }
	//save screen buffer into "old screen buffer"
    for(x=0;x<(64*32);x++)
		chip->memory->old_screen[x]=chip->memory->screen[x];

    FlipScreen();

}
//free memory
void _Free(struct _Chip *chip){
    if(chip){
        free(chip->memory);
        free(chip);
     }
}
//init cpu
void Chip_8(struct _Chip **chip){
     //alloc memory
    *chip=malloc(sizeof(struct _Chip));
    (*chip)->memory=malloc(sizeof(struct _Memory));
    //set pointers
    (*chip)->Execute=_Execute;
    (*chip)->MakeRom=_MakeRom;
    (*chip)->SetRom=_SetRom;
    (*chip)->Free=_Free;
    (*chip)->Draw=_Draw;
    //init all values
    _ZeroCpu((*chip));

}

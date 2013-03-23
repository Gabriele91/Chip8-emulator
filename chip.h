/*!
 * \author Gabriele Di Bari
 * \date 2011/06/15
 * \details translation comments on 2013/03/23
 */
#ifndef CHIP_H
#define CHIP_H
#define _DEBUG_
//#define _LOG_OUT
#include <stdlib.h>

/*!
 * \struct event management
 */
typedef struct _Event{
    unsigned short (*KeyPress)(int i);
    unsigned short (*Wait)();
}Event;
/*!
 * \struct chip memory
 */
struct _Memory;
typedef struct _Memory Memory;
/*!
 * \struct rom
 */
struct _Rom;
typedef struct _Rom Rom;
/*!
 * \struct chip
 */
struct _Chip;
typedef struct _Chip{
    /*!
     * \var Memory* memory
     * \brief cpu memory
     */
    Memory* memory;
    /*!
     * \var Event* input
     * \brief input event
     */
    Event*  input;
    /*!
     * \fn MakeRom
     * \brief load rom file and return memory arrocation
     * \param file a const char*
     * \return rom a struct
     * \see rom
     */
    Rom* (*MakeRom)(const char* file);
    /*!
     * \fn SetRom
     * \brief set the program pointer that points to the rom
     * \param chip a struct _Chip *
     * \param rom a Rom struct
     * \see rom
     */
    void (*SetRom)(struct _Chip *chip,Rom *rom);
    /*!
     * \fn Execute
     * \brief execute rom binary code
     * \param chip a struct _Chip *
     * \return  a integer
     */
    int (*Execute)(struct _Chip *chip);
    /*!
     * \fn Draw
     * \brief draw on screen buffer
     * \param chip a struct _Chip *
     */
    void (*Draw)(struct _Chip *chip);
    /*!
     * \fn Free
     * \brief delete a chip
     * \param chip a struct _Chip *
     */
    void (*Free)(struct _Chip *chip);
}Chip;

/*!
 * \fn Chip_8
 * \brief initializes the cpu
 * \param chip a struct _Chip **
 */
void Chip_8(struct _Chip **chip);
#endif

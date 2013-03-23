/*!
 * \author Gabriele Di Bari
 * \date 2011/06/15
 * \details translation comments on 2013/03/23
 */
#ifndef WINDOW_H
#define WINDOW_H

#define false 0
#define true 1
extern unsigned char char_key[256];
extern unsigned char closeWindow;

/*!
 * \fn StartWindow
 * \brief open a window
 * \param sizex an integer
 * \param sizey an integer
 */
void StartWindow(int sizex,int sizey);
/*!
 * \fn GetWW
 * \brief get window Width
 * \return an integer
 */
int GetWW();
/*!
 * \fn GetWH
 * \brief window Height
 * \return an integer
 */
int GetWH();
/*!
 * \fn UpDataWindow
 * \brief updata window
 */
void UpDataWindow();
/*!
 * \fn FlipScreen
 * \brief draw on window
 */
void FlipScreen();
/*!
 * \fn CloseTheWindow
 * \brief close window
 */
void CloseTheWindow();

#endif

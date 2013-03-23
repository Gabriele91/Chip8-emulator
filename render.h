/*!
 * \author Gabriele Di Bari
 * \date 2011/06/15
 * \details translation comments on 2013/03/23
 */
#ifndef RENDER_H
#define RENDER_H
/*!
 * \fn Scale
 * \brief set scale
 * \param x a float
 * \param y a float
 */
void Scale(float x,float y);
/*!
 * \fn DrawPoint
 * \brief draw a point
 * \param x an integer
 * \param y an integer
 */
void DrawPoint(int x,int y);
/*!
 * \fn Color
 * \brief set color
 * \param r an integer
 * \param g an integer
 * \param b an integer
 */
void Color(int r,int g,int b);
/*!
 * \fn Cls
 * \brief clears the screen with a color
 * \param r an integer
 * \param g an integer
 * \param b an integer
 */
void Cls(int r,int g,int b);

#endif

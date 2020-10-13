////////////////
// 
// File: kmeans.h
//
//  Defines type `point_t` and `color_t` for K-Means simulation.
//
//  * You may add extra declarations if you need.
//
// Jose @ ShanghaiTech University
//
////////////////

#ifndef _KMEANS_H_
#define _KMEANS_H_

#include <iostream>
#include <iomanip>


/*********************************************************
      Your extra "include"s if needed in this header
 *********************************************************/

/*********************************************************
                           End
 *********************************************************/


/*
 * Structure of a data point.
 * 
 *   Fields:
 *     x - double, the x coordinate value.
 *     y - double, the y coordinate value.
 *
 *   Member functions:
 *     getX() / getY(): Acquire x or y coordinate.
 *     setXY(xi, yi): Assign a new (x, y) position.
 */
typedef struct point_t
{
    double x;
    double y;

    /* Constructors. */
    point_t() : x(0), y(0) {};
    point_t(double xi, double yi) : x(xi), y(yi) {};

    /* Acquire x or y coordinate. */
    double getX(void) const { return x; };
    double getY(void) const { return y; };

    /* Assign a new (x, y) position. */
    void setXY(double xi, double yi) {
        x = xi;
        y = yi;
    };
} point_t;

/* Overload `<<` for prettier printing. */
std::ostream&
operator<< (std::ostream& os, const point_t& p)
{
    os.setf(std::ios::fixed);
    os << std::setw(13) << std::setprecision(8) << p.getX() << ", "
       << std::setw(13) << std::setprecision(8) << p.getY();
    return os;
}

/*
 * Specify `color_t` as unsinged 8-bit integer, which represents
 * the color code of a data point.
 *
 */
typedef uint8_t color_t;

/* Overload `<<` for correct printing. */
std::ostream&
operator<< (std::ostream& os, const color_t& c)
{
    os << (unsigned) c;
    return os;
}

/*
 * Function prototypes.
 *
 */
void kmeans (point_t * const data, point_t * const mean,
             color_t * const coloring, const int pn, const int cn);


/*********************************************************
           Your extra declarations can go here
 *********************************************************/

/*********************************************************
                           End
 *********************************************************/


#endif

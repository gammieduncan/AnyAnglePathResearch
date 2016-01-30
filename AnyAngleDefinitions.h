/*
This code was developed by Tansel Uras (turas@usc.edu) at USC.
The code is hosted at 'http://idm-lab.org/anyangle'.
If you use this code in your research, please  cite our SoCS paper:

T. Uras and S. Koenig,  2015. An Empirical Comparison of Any-Angle Path-Planning Algorithms. In: Proceedings of the 8th Annual Symposium on Combinatorial
Search. Code available at: http://idm-lab.org/anyangle

Bibtex:
@inproceedings{uras:15,
  author = "T. Uras and S. Koenig",
  title = "An Empirical Comparison of Any-Angle Path-Planning Algorithms",
  booktitle = {Proceedings of the 8th Annual Symposium on Combinatorial Search},
  year = "2015",
  note = "Code available at: http://idm-lab.org/anyangle",
}
*/

// xyLoc : grid cell (without any padding)
// CornerLoc : grid corner
// xyLocCont : any point on the grid

#ifndef ANY_ANGLE_DEFINITIONS_H
#define ANY_ANGLE_DEFINITIONS_H

#ifndef NO_HOG  // Use '-DNO_HOG' in the makefile to compile this outside the HOG platform.
#define ANY_ANGLE_RUNNING_IN_HOG    // Otherwise, the program assumes that it is running in HOG, which enables a specific constructor and visualization for HOG.
//#include "Map2DEnvironment.h"
//#include "FPUtil.h"
#endif

#include <string>

#define ANY_ANGLE_STATISTICS		// Keep statistics and write them in a file

#ifdef ANY_ANGLE_RUNNING_IN_HOG
//#define ANY_ANGLE_ASSERTIONS		// Not necessary, just for debugging
#endif

#define DISALLOW_MOVES_BETWEEN_DIAGONAL_OBSTACLES

// Some definitions that are used by the base class or some/all of the derived classes.

// Movement costs.
typedef double cost;
#define INFINITE_COST std::numeric_limits<cost>::max()
#define CARD_COST 1
#define DIAG_COST 1.414213
#define DIAG_DIFF 0.414213

// After this many searches, reset the generated values.
#define MAX_SEARCH 50000

// Location of a state in search.
#define NO_LIST 0
#define OPEN_LIST 1
#define CLOSED_LIST 2

// Directions
typedef uint32_t direction;

#define DIR_N 0
#define DIR_NE 1
#define DIR_E 2
#define DIR_SE 3
#define DIR_S 4
#define DIR_SW 5
#define DIR_W 6
#define DIR_NW 7

// If the difference between two floats is no more than this value, we treat them as the same value.
#define EPSILON 0.00001

// Corners and all search related information about corners are kept in an array. Specific corners are found in the array by their cornerIds.
typedef uint32_t cornerId;

// Some search methods do not use corners as states (such as ANYA), in which case, they might need to create an array that stores states.
typedef uint64_t stateId;

#ifndef ANY_ANGLE_RUNNING_IN_HOG
// Redefine xyLoc as in HOG
struct xyLoc {
public:
    xyLoc() {}
    xyLoc(uint16_t _x, uint16_t _y) :x(_x), y(_y) {}
    uint16_t x;
    uint16_t y;
};

static bool operator==(const xyLoc &l1, const xyLoc &l2) {
    return (l1.x == l2.x) && (l1.y == l2.y);
}

#endif

#ifdef DISALLOW_MOVES_BETWEEN_DIAGONAL_OBSTACLES
// kNotACorner is used for xyLocCont
enum CornerLocType {kCorner, kCornerNorthWest, kCornerNorthEast, kCornerSouthEast, kCornerSouthWest, kNotACorner};
#endif

#define NO_CORNER std::numeric_limits<cornerId>::max()

// If such movements are disallowed, then there can we must treat the same corner as two different corners.

struct Cell {
    Cell() :
        northeast_corner(NO_CORNER), southeast_corner(NO_CORNER),
        northwest_corner(NO_CORNER), southwest_corner(NO_CORNER), is_traversable(false)
    {}

    cornerId northeast_corner;
    cornerId southeast_corner;
    cornerId northwest_corner;
    cornerId southwest_corner;

    bool is_traversable;
};

struct CornerLoc {
public:
    CornerLoc() {}

#ifdef DISALLOW_MOVES_BETWEEN_DIAGONAL_OBSTACLES
    CornerLoc(uint16_t _x, uint16_t _y, CornerLocType _type = kCorner) :x(_x), y(_y), type(_type){}
#else
    CornerLoc(uint16_t _x, uint16_t _y) :x(_x), y(_y){}
#endif

    uint16_t x;
    uint16_t y;

#ifdef DISALLOW_MOVES_BETWEEN_DIAGONAL_OBSTACLES
    CornerLocType type;
#endif
};

static bool operator==(const CornerLoc &l1, const CornerLoc &l2) {
#ifndef DISALLOW_MOVES_BETWEEN_DIAGONAL_OBSTACLES
    return (l1.x == l2.x) && (l1.y == l2.y);
#else
    return (l1.x == l2.x) && (l1.y == l2.y) && (l1.type == l2.type);
#endif
}

static std::ostream& operator <<(std::ostream & out, const CornerLoc &loc)
{
    out << "(" << loc.x << ", " << loc.y << ")";
#ifdef DISALLOW_MOVES_BETWEEN_DIAGONAL_OBSTACLES:
    if (loc.type == kCorner)
        out<<"-corner";
    if (loc.type == kCornerNorthWest)
        out<<"-cornerNW";
    if (loc.type == kCornerNorthEast)
        out<<"-cornerNE";
    if (loc.type == kCornerSouthWest)
        out<<"-cornerSW";
    if (loc.type == kCornerSouthEast)
        out<<"-cornerSE";
#endif
    return out;
}


// Different from xyLoc, xyLocCont allows the x and y coordinates to be any float value.
struct xyLocCont {
    float x;
    float y;

#ifdef DISALLOW_MOVES_BETWEEN_DIAGONAL_OBSTACLES
    CornerLocType type;

    xyLocCont(float _x = 0, float _y = 0, CornerLocType _type = kNotACorner) :
        x(_x), y(_y), type(_type)
    {
    }
    xyLocCont(CornerLoc l) :
        x(l.x), y(l.y), type(l.type)
    {}

//  We don't want this conversion, since
//      xyLocCont(xyLoc l):
//        x(l.x), y(l.y)
//    {}

    // Rounds down the x and y values to return an xyLoc. Should only be used if it is known that the continuous values correspond to integers.
 //   CornerLoc getCorner()   // TODO: Move this function to the AnyAlgorithm class, so that it can determine the type of the CornerLoc
 //   {
 //       return CornerLoc((int) (x + EPSILON), (int) (y + EPSILON));
 //   }

    // Returns true if the respective x and y coordinates of two xyLocCont variables differ by at most EPSILON.
    bool operator==(const xyLocCont& lhs) const
    {
        return (x <= lhs.x + EPSILON && x + EPSILON >= lhs.x) && (y <= lhs.y + EPSILON && y + EPSILON >= lhs.y && type == lhs.type);
    }

    bool operator==(const CornerLoc& lhs) const
    {
        return (x <= lhs.x + EPSILON && x + EPSILON >= lhs.x) && (y <= lhs.y + EPSILON && y + EPSILON >= lhs.y && type == lhs.type);
    }

#else
    xyLocCont(float _x = 0, float _y = 0) :
        x(_x), y(_y)
    {
    }
    xyLocCont(CornerLoc l) :
        x(l.x), y(l.y)
    {}
    xyLocCont(xyLoc l):
        x(l.x), y(l.y)
    {}

    // Rounds down the x and y values to return an xyLoc. Should only be used if it is known that the continuous values correspond to integers.
    CornerLoc getCorner()   // TODO: Move this function to the AnyAlgorithm class, so that it can determine the type of the CornerLoc
    {
        return CornerLoc((int) (x + EPSILON), (int) (y + EPSILON));
    }

    // Returns true if the respective x and y coordinates of two xyLocCont variables differ by at most EPSILON.
    bool operator==(const xyLocCont& lhs) const
    {
        return (x <= lhs.x + EPSILON && x + EPSILON >= lhs.x) && (y <= lhs.y + EPSILON && y + EPSILON >= lhs.y);
    }
#endif
};


#endif

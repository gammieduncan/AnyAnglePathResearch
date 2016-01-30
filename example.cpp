#include <cheerp/client.h>
#include <cheerp/clientlib.h>
#include <stdio.h>
#include <stdint.h>
#include <numeric>
#include <algorithm>
#include <string>
#include <iostream>
#include <deque>
#include <queue>
#include <vector>
#include <fstream>
#include <algorithm>
#include <limits>
#include <math.h>
//#include "Timer.h"

//Start Theta Star Dependencies
/*Start AnyAngleDefinitions.h*/
#include <string>

#define ANY_ANGLE_STATISTICS		// Keep statistics and write them in a file
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

// kNotACorner is used for xyLocCont
enum CornerLocType {kCorner, kCornerNorthWest, kCornerNorthEast, kCornerSouthEast, kCornerSouthWest, kNotACorner};

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
    CornerLoc(uint16_t _x, uint16_t _y, CornerLocType _type = kCorner) :x(_x), y(_y), type(_type){}
    uint16_t x;
    uint16_t y;
    CornerLocType type;
};

static bool operator==(const CornerLoc &l1, const CornerLoc &l2) {
    return (l1.x == l2.x) && (l1.y == l2.y);
}
static std::ostream& operator <<(std::ostream & out, const CornerLoc &loc)
{
    out << "(" << loc.x << ", " << loc.y << ")";
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
    return out;
}


// Different from xyLoc, xyLocCont allows the x and y coordinates to be any float value.
struct xyLocCont {
    float x;
    float y;

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
};
/*End AnyAngleDefinitions.h*/
/*Start AnyAngleAlgorithms.h*/
// Vertices are placed at the corners of cells.
// Given a map of size n x m (n x m cells, n+1 x m+1 corners):
// Top left cell is (0,0), top left corner is (0,0), and corner (x,y) is located at the top left corner of cell (x,y).
//
// We add a frame of obstacles around the map but don't add any more corners, resulting in n+2 x m+2 cells, n+1 x m+1 corners.
//
// Now, corner x,y is located at the bottom right corner of cell x,y, and every corner is surrounded by cells so we don't have to do any out-of-map-bounds
// checks.

class AnyAngleAlgorithm {

public:
	AnyAngleAlgorithm(const std::vector<bool> &bits, const int width, const int height);
	~AnyAngleAlgorithm();

	// Finds an any-angle path between the two xyLoc's and returns the cost. Cost is infinite if there is no path.
	// Automatically validates the path, smoothes the path (if enabled) and reports statistics (if enabled).
	// Calls either FindXYLocPath(...) or FindXYLocContPath(...) depending on the value of UsingXYLocCont().
	cost FindPath(const xyLoc from, const xyLoc to);

	// Any derived class should implement either indXYLocPath(...) or FindXYLocContPath(...) and set the value of UsingXYLocCont() accordingly.
	// xyLoc have integer coordinates, whereas xyLocCont have float coordinates.
	// For instance, Field A* finds xyLocCont paths because the midpoints of the paths can be any point on the boundary of a grid cell.
	virtual cost FindCornerLocPath(xyLoc from, xyLoc to, std::vector<CornerLoc> &path)  {return INFINITE_COST;}
    virtual cost FindXYLocContPath(xyLoc from, xyLoc to, std::vector<xyLocCont> &path)  {return INFINITE_COST;}

    // By default, we assume anyangle algorithms only use corner locations as midpoints in the paths and the paths are smoothed afterwards.
    // Any implementation that differs from this norm should override the relevant functions below.
    virtual const bool UsingXYLocCont() const {return false;}
    virtual const bool ShouldSmoothPaths() const {return true;}

    // Returns a (unique) name to identify the algorithm.
    // Used for setting up the statistics file. Is not called during construction, so the algorithms can generate names based on their parameters (if any).
    virtual const std::string GetName() const = 0;

    // Generates a random problem with valid start and goal locations (solution might not exist).
    void GetRandomProblem(xyLoc &l1, xyLoc &l2) const;

protected:

    // TODO
    xyLoc ToXYLoc(const CornerLoc loc) const {return xyLoc(loc.x, loc.y);}

    // Returns the cornerId of the given xyLoc.
    cornerId ToCornerId(const uint16_t x, const uint16_t y) const;
    cornerId ToCornerId(const xyLoc loc) const {return ToCornerId(loc.x, loc.y);}
    cornerId ToCornerId(const CornerLoc & l) const;

    // Returns the id of the corner that is arrived at x,y, when approached by an agent traveling in direction 'dir'.
    cornerId ToCornerId(const uint16_t x, const uint16_t y, int dir) const;

    CornerLoc ToCornerLoc(const xyLoc loc) const {return corner_locations_[ToCornerId(loc)];}
    CornerLoc ToCornerLoc(const cornerId c) const {return corner_locations_[c];}

    // Returns the xyLoc of the given cornerId
    CornerLoc ToXYLoc(const cornerId id) const {return CornerLoc(corner_locations_[id].x, corner_locations_[id].y);}


	// Dimensions of the map, after adding a frame of blocked cells. That is, the map has height x width cells.
	// It has (height - 1) * (width - 1) corners, each surrounded by four blocked cells.
	unsigned int height_, width_;

	// Identifies whether each cell is blocked (= 0) or not (= 1)
    Cell** cells_;

	// Stores the cornerId of each valid corner (that is, any grid corner not surrounded by 4 blocked cells). -1 if the corner is surrounded by 4 blocked cells.
	cornerId** corner_ids_;

	// For each cornerId, stores the neighbor in all 8 directions (NO_CORNER if none exists in a given direction).
	cornerId** corner_neighbors_;

	// Reverse mapping of corner_ids_. corner_locations_[i] gives the location of the corner with id 'i'.
	std::vector<CornerLoc> corner_locations_;

	// A list of all corners whose surrounding cells contain at least one unblocked cell (used for generating random instances).
	std::vector<xyLoc> unblocked_cells_;

	// Determines whether the HeuristicDistance function returns the octile distance or the Euclidean distance.
	bool using_octile_distance_;

	// FindPath function stores the paths in the following variables.
	std::vector<CornerLoc> xyloc_path_, smoothed_xyloc_path_;
	std::vector<xyLocCont> xyloc_cont_path_, smoothed_xyloc_cont_path_;


	// Start and goal of the current search (for visualization and path validation)
	xyLoc from_, to_;

	// Called by both the HOG and non-HOG constructor to add a frame of obstacles to the map and assign cornerIds.
	void Initialize();

	void ComputeCornerNeighbors();
	std::vector<cornerId> GetCornerNeighbors(const cornerId c) const;
	cornerId GetCornerNeighbor(const cornerId c, const direction d) const {return corner_neighbors_[c][d];};

	// Returns all the neighbors of a grid corner that can be reached by a straight line with cost 1 or sqrt(2).
	std::vector<cornerId> GetNeighbors(const cornerId c) const;
	std::vector<CornerLoc> GetNeighbors(const CornerLoc l) const;

	// Returns true if the straight line between the two points do not pass through the interior of an obstacle.
	// The straight line cannot pass between two blocked cells that share an edge, but can pass between two blocked cells that share only a corner.
	bool LineOfSight(CornerLoc l1, CornerLoc l2);
	bool LineOfSight(xyLocCont l1, xyLocCont l2);
	bool LineOfSight(cornerId c1, cornerId c2) {return LineOfSight(corner_locations_[c1], corner_locations_[c2]);}

	// Smooths a path by performing line of sight checks linear in the number of points on the original non-smooothed path.
	cost SmoothPath(const std::vector<CornerLoc> &path, std::vector<CornerLoc> &smoothed_path);
	cost SmoothPath(const std::vector<xyLocCont> &path, std::vector<xyLocCont> &smoothed_path);

    // Returns true if the cell is unblocked
    bool IsTraversable(const xyLoc l) const    {return cells_[l.x][l.y].is_traversable;}
    bool IsTraversable(const unsigned int x, const unsigned int y) const   {return cells_[x][y].is_traversable;}

	// Returns the neighboring cell of a corner, specified by the direction in function's name.
	// Corner (x,y)'s surrounding cells are (x,y), (x+1,y), (x,y+1), (x+1,y+1).
	xyLoc NorthWestCell(const unsigned int x, const unsigned int y) const {return xyLoc(x,y);}
	xyLoc NorthEastCell(const unsigned int x, const unsigned int y) const {return xyLoc(x+1,y);}
	xyLoc SouthWestCell(const unsigned int x, const unsigned int y) const {return xyLoc(x,y+1);}
	xyLoc SouthEastCell(const unsigned int x, const unsigned int y) const {return xyLoc(x+1,y+1);}

    xyLoc NorthWestCell(const CornerLoc l) const {return NorthWestCell(l.x,l.y);}
    xyLoc NorthEastCell(const CornerLoc l) const {return NorthEastCell(l.x,l.y);}
    xyLoc SouthWestCell(const CornerLoc l) const {return SouthWestCell(l.x,l.y);}
    xyLoc SouthEastCell(const CornerLoc l) const {return SouthEastCell(l.x,l.y);}

	xyLoc NorthWestCell(const cornerId id) const {return NorthWestCell(ToXYLoc(id));}
	xyLoc NorthEastCell(const cornerId id) const {return NorthEastCell(ToXYLoc(id));}
	xyLoc SouthWestCell(const cornerId id) const {return SouthWestCell(ToXYLoc(id));}
	xyLoc SouthEastCell(const cornerId id) const {return SouthEastCell(ToXYLoc(id));}

	// Returns true if the corner is a convex corner of an obstacle (that is, a visibility graph vertex).
	bool IsConvexCorner(const CornerLoc l) const
        {return ((IsTraversable(NorthWestCell(l)) && IsTraversable(SouthEastCell(l))) && !(IsTraversable(NorthEastCell(l)) && IsTraversable(SouthWestCell(l))))
			|| (!(IsTraversable(NorthWestCell(l)) && IsTraversable(SouthEastCell(l))) && (IsTraversable(NorthEastCell(l)) && IsTraversable(SouthWestCell(l))));}
	bool IsConvexCorner(const unsigned int x, const unsigned int y) const  {return IsConvexCorner(CornerLoc(x,y));}
	bool IsConvexCorner(const cornerId c) const  {return IsConvexCorner(corner_locations_[c]);}

	bool CornerAtDiagonalIntersection(const int x, const int y) const
	{   bool nw = cells_[x][y].is_traversable;     bool ne = cells_[x+1][y].is_traversable;
	    bool sw = cells_[x][y+1].is_traversable;   bool se = cells_[x+1][y+1].is_traversable;
	    return (nw && se && !ne && !sw) || (!nw && !se && ne && sw);
	}
	bool CornerAtDiagonalIntersection(const CornerLoc & corner) const {return CornerAtDiagonalIntersection(corner.x, corner.y);}
	bool CornerAtDiagonalIntersection(const cornerId c) const {return CornerAtDiagonalIntersection(corner_locations_[c]);}

	// Returns x such that (x,y), (x1,y1) and (x2,y2) are colinear.
	double GetIntersectingX(const double x1, const double y1, const double x2, const double y2, const double y) const
        {return (fabs(y2-y1) < EPSILON) ? y1 : ((x2-x1)/(y2-y1))*(y-y1) + x1;}

    // Returns y such that (x,y), (x1,y1) and (x2,y2) are colinear.
	double GetIntersectingY(const double x1, const double y1, const double x2, const double y2, const double x) const
        {return (fabs(x2-x1) < EPSILON) ? x1 : ((y2-y1)/(x2-x1))*(x-x1) + y1;}

	// Returns true if the interval (f1, f2) or (f2, f1) contains an integer value.
	bool IntervalContainsInteger(const double f1, const double f2) const
        {if (fabs(f1 -f2) < EPSILON) return false;	// f1 = f2: The interval is empty.
		return (f1 < f2) ? (ceil(f1 - EPSILON) == floor(f2 + EPSILON)) : (ceil(f2 - EPSILON) == floor(f1 + EPSILON));}

	// Returns true if (x1,y1), (x2,y2) and (x3,y3) are colinear.
	bool CoLinear(const double x1, const double y1, const double x2, const double y2, const double x3, const double y3) const
        {return fabs((y2-y1)*(x3-x2) - (x2-x1)*(y3-y2)) < EPSILON;}

	// Returns the Octile distance between two corners.
	cost OctileDistance(const CornerLoc l1, const CornerLoc l2) const
		{int dx = (l1.x>l2.x)?(l1.x-l2.x):(l2.x-l1.x);	int dy = (l1.y>l2.y)?(l1.y-l2.y):(l2.y-l1.y);
		return (dx>dy)?(dx*CARD_COST + dy*DIAG_DIFF):(dy*CARD_COST + dx*DIAG_DIFF);}

    cost OctileDistance(const cornerId c1, const cornerId c2) const
        {return OctileDistance(corner_locations_[c1], corner_locations_[c2]);}

    // Returns the Euclidean distance between two points.
	cost EuclideanDistance(const CornerLoc l1, const CornerLoc l2) const
		{double dx = (l2.x-l1.x);	double dy = (l2.y-l1.y);	return sqrt((cost)dx*dx + dy*dy);}

	cost EuclideanDistance(const double x1, const double y1, const double x2, const double y2) const
		{double dx = (x2-x1);	double dy = (y2-y1);	return sqrt((cost)dx*dx + dy*dy);}

	cost EuclideanDistance(const xyLocCont l1, const xyLocCont l2) const
		{double dx = (l2.x-l1.x);	double dy = (l2.y-l1.y);	return sqrt((cost)dx*dx + dy*dy);}

	cost EuclideanDistance(const cornerId c1, const cornerId c2) const
        {return EuclideanDistance(corner_locations_[c1], corner_locations_[c2]);}

	// Returns either the Octile or the Euclidean distance, depending on the value of 'using_octile_distance_'.
	cost HeuristicDistance(const CornerLoc l1, const CornerLoc l2) const
        {return using_octile_distance_?OctileDistance(l1,l2):EuclideanDistance(l1,l2);}

	cost HeuristicDistance(const cornerId c1, const cornerId c2) const
        {return HeuristicDistance(corner_locations_[c1], corner_locations_[c2]);}

	// Sets which heuristic function to use.
	void UseOctileDistance(){using_octile_distance_ = true;}
	void UseEuclideanDistance(){using_octile_distance_ = false;}
};
/*End AnyAngleAlgorithms.h*/
/*Start AnyAngleAlgorithm.cpp*/
AnyAngleAlgorithm::AnyAngleAlgorithm(const std::vector<bool> &bits, int width, int height)
{
    // Read the height and width of the map.
    width_ = width + 2; // Add +2 for the frame of obstacles.
    height_ = height + 2;

    unblocked_cells_.clear();

    // Generate the cells with the padding.
    cells_ = new Cell*[width_];
    for (unsigned int x = 0; x < width_; x++) {
        cells_[x] = new Cell[height_];
    }

    // Make unblocked cells traversable.
    int num_traversable = 0;
    for (unsigned int x = 0; x < width_ - 2; x++) {
        for (unsigned int y = 0; y < height_ - 2; y++) {
            if (bits[y * width + x]) {
                cells_[x + 1][y + 1].is_traversable = true;
                unblocked_cells_.push_back(xyLoc(x,y));
            }
        }
    }

    Initialize();
}
AnyAngleAlgorithm::~AnyAngleAlgorithm()
{
    for (unsigned int x = 0; x < width_ - 1; x++) {
        delete[] cells_[x];
        delete[] corner_ids_[x];
    }
    delete[] cells_[width_ - 1];

    delete[] cells_;
    delete[] corner_ids_;
}
void AnyAngleAlgorithm::Initialize()
{

    // Generate 'corner_ids_' with the padding.
    corner_ids_ = new cornerId*[width_ - 1];
    for (unsigned int x = 0; x < width_ - 1; x++) {
        corner_ids_[x] = new cornerId[height_ - 1];
    }

    // Determine the corners and create the two-way mapping between cornerIds and corner locations.
    corner_locations_.clear();

    for (unsigned int y = 0; y < height_ - 1; y++) {    // Ignore the last row and the last column
        for (unsigned int x = 0; x < width_ - 1; x++) {
            Cell* northwest_cell = &cells_[x][y];
            Cell* northeast_cell = &cells_[x+1][y];
            Cell* southeast_cell = &cells_[x+1][y+1];
            Cell* southwest_cell = &cells_[x][y+1];

            bool nw = northwest_cell->is_traversable;
            bool ne = northeast_cell->is_traversable;
            bool se = southeast_cell->is_traversable;
            bool sw = southwest_cell->is_traversable;

            // If it has exactly two blocked neighbors that are diagonally adjacent, generate two vertices for it.
            if (nw && se && !ne && !sw) {
                northwest_cell->southeast_corner = corner_locations_.size();
                corner_locations_.push_back(CornerLoc(x, y, kCornerNorthWest));

                southeast_cell->northwest_corner = corner_locations_.size();
                corner_locations_.push_back(CornerLoc(x, y, kCornerSouthEast));
            }
            else if (!nw && !se && ne && sw) {
                northeast_cell->southwest_corner = corner_locations_.size();
                corner_locations_.push_back(CornerLoc(x, y, kCornerNorthEast));

                southwest_cell->northeast_corner = corner_locations_.size();
                corner_locations_.push_back(CornerLoc(x, y, kCornerSouthWest));
            }

            // Otherwise, generate a single vertex for it.
            else if (nw || se || ne || sw) {
                if (nw)
                    northwest_cell->southeast_corner = corner_locations_.size();
                if (se)
                    southeast_cell->northwest_corner = corner_locations_.size();
                if (ne)
                    northeast_cell->southwest_corner = corner_locations_.size();
                if (sw)
                    southwest_cell->northeast_corner = corner_locations_.size();

                corner_locations_.push_back(CornerLoc(x, y, kCorner));
            }
            CornerLoc loc = CornerLoc(x, y); // Location of the corner, not the cell

            corner_ids_[x][y] = corner_locations_.size();
            corner_locations_.push_back(loc);
/*
            // A corner is a 'valid' corner if one of its four surrounding cells is traversable
            if (IsTraversable(NorthEastCell(loc)) || IsTraversable(NorthWestCell(loc)) ||
                IsTraversable(SouthEastCell(loc)) || IsTraversable(SouthWestCell(loc))) {
                unblocked_cells_.push_back(loc);
            }
*/
        }
    }


    ComputeCornerNeighbors();

    using_octile_distance_ = false;
}

cornerId AnyAngleAlgorithm::ToCornerId(const uint16_t x, const uint16_t y) const
{
    return corner_ids_[x][y];
}
cornerId AnyAngleAlgorithm::ToCornerId(const CornerLoc & loc) const
{
    // TODO
    switch (loc.type) {
    case kCornerNorthWest:
        return cells_[loc.x][loc.y].southeast_corner;
    case kCornerNorthEast:
        return cells_[loc.x+1][loc.y].southwest_corner;
    case kCornerSouthEast:
        return cells_[loc.x+1][loc.y+1].northwest_corner;
    case kCornerSouthWest:
        return cells_[loc.x][loc.y+1].northeast_corner;
    case kCorner:
        if (cells_[loc.x][loc.y].southeast_corner != NO_CORNER)
            return cells_[loc.x][loc.y].southeast_corner;
        if (cells_[loc.x+1][loc.y].southwest_corner != NO_CORNER)
            return cells_[loc.x+1][loc.y].southwest_corner;
        if (cells_[loc.x+1][loc.y+1].northwest_corner != NO_CORNER)
            return cells_[loc.x+1][loc.y+1].northwest_corner;
        if (cells_[loc.x][loc.y+1].northeast_corner != NO_CORNER)
            return cells_[loc.x][loc.y+1].northeast_corner;
        break;
    default:
        return NO_CORNER;
    }
}

cornerId AnyAngleAlgorithm::ToCornerId(const uint16_t x, const uint16_t y, int dir) const
{
    Cell* northwest_cell = &cells_[x][y];
    Cell* northeast_cell = &cells_[x+1][y];
    Cell* southeast_cell = &cells_[x+1][y+1];
    Cell* southwest_cell = &cells_[x][y+1];

    bool nw = northwest_cell->is_traversable;
    bool ne = northeast_cell->is_traversable;
    bool se = southeast_cell->is_traversable;
    bool sw = southwest_cell->is_traversable;

    if (se && (dir == DIR_N || dir == DIR_W || dir == DIR_NW))
        return southeast_cell->northwest_corner;

    if (nw && (dir == DIR_S || dir == DIR_E || dir == DIR_SE))
        return northwest_cell->southeast_corner;

    if (sw && (dir == DIR_N || dir == DIR_E || dir == DIR_NE))
        return southwest_cell->northeast_corner;

    if (ne && (dir == DIR_S || dir == DIR_W || dir == DIR_SW))
        return northeast_cell->southwest_corner;

    return NO_CORNER;
}

cost AnyAngleAlgorithm::FindPath(xyLoc from, xyLoc to)
{

    cost c;
    if (!UsingXYLocCont()) {
        xyloc_path_.clear();
        c = FindCornerLocPath(from, to, xyloc_path_);

        if (ShouldSmoothPaths()) {
            c = SmoothPath(xyloc_path_, smoothed_xyloc_path_);
        }
    }
    else {
        xyloc_cont_path_.clear();
        c = FindXYLocContPath(from, to, xyloc_cont_path_);
        if (ShouldSmoothPaths()) {
            c = SmoothPath(xyloc_cont_path_, smoothed_xyloc_cont_path_);
        }
    }

    return c;
}

void AnyAngleAlgorithm::ComputeCornerNeighbors()
{
    corner_neighbors_ = new cornerId*[corner_locations_.size()];
    for (cornerId c = 0; c < corner_locations_.size(); c++)
    {
        corner_neighbors_[c] = new cornerId[8];
        for (int i = 0; i < 8; i++)
            corner_neighbors_[c][i] = NO_CORNER;

        int x = corner_locations_[c].x;
        int y = corner_locations_[c].y;
        CornerLocType type = corner_locations_[c].type;

        Cell* northwest_cell = &cells_[x][y];
        Cell* northeast_cell = &cells_[x+1][y];
        Cell* southeast_cell = &cells_[x+1][y+1];
        Cell* southwest_cell = &cells_[x][y+1];

        bool nw = northwest_cell->is_traversable;
        bool ne = northeast_cell->is_traversable;
        bool se = southeast_cell->is_traversable;
        bool sw = southwest_cell->is_traversable;

        if (nw && (type == kCornerNorthWest || type == kCorner)) {
            corner_neighbors_[c][DIR_N] = northwest_cell -> northeast_corner; // North
            corner_neighbors_[c][DIR_W] = northwest_cell -> southwest_corner; // West
            corner_neighbors_[c][DIR_NW] = northwest_cell -> northwest_corner; // NorthWest
        }

        if (ne && (type == kCornerNorthEast || type == kCorner)) {
            corner_neighbors_[c][DIR_N] = northeast_cell -> northwest_corner;
            corner_neighbors_[c][DIR_E] = northeast_cell -> southeast_corner;
            corner_neighbors_[c][DIR_NE] = northeast_cell -> northeast_corner;
        }

        if (se && (type == kCornerSouthEast || type == kCorner)) {
            corner_neighbors_[c][DIR_E] = southeast_cell -> northeast_corner;
            corner_neighbors_[c][DIR_S] = southeast_cell -> southwest_corner;
            corner_neighbors_[c][DIR_SE] = southeast_cell -> southeast_corner;
        }

        if (sw && (type == kCornerSouthWest || type == kCorner)) {
            corner_neighbors_[c][DIR_W] = southwest_cell -> northwest_corner;
            corner_neighbors_[c][DIR_S] = southwest_cell -> southeast_corner;
            corner_neighbors_[c][DIR_SW] = southwest_cell -> southwest_corner;
        }
    }
}

std::vector<cornerId> AnyAngleAlgorithm::GetCornerNeighbors(const cornerId c) const
{
    std::vector<cornerId> neighbors;
    for (int i = 0; i < 8; i++)
        neighbors.push_back(corner_neighbors_[c][i]);
    return neighbors;
}

std::vector<cornerId> AnyAngleAlgorithm::GetNeighbors(const cornerId c) const
{
    std::vector<cornerId> neighbors;

    CornerLoc l = corner_locations_[c];

    // NorthWest
    if (IsTraversable(NorthWestCell(l)))
        neighbors.push_back(corner_ids_[l.x - 1][l.y - 1]);

    // NorthEast
    if (IsTraversable(NorthEastCell(l)))
        neighbors.push_back(corner_ids_[l.x + 1][l.y - 1]);

    // SouthWest
    if (IsTraversable(SouthWestCell(l)))
        neighbors.push_back(corner_ids_[l.x - 1][l.y + 1]);

    // SouthEast
    if (IsTraversable(SouthEastCell(l)))
        neighbors.push_back(corner_ids_[l.x + 1][l.y + 1]);

    // North
    if (IsTraversable(NorthWestCell(l)) || IsTraversable(NorthEastCell(l)))
        neighbors.push_back(corner_ids_[l.x][l.y - 1]);

    // South
    if (IsTraversable(SouthWestCell(l)) || IsTraversable(SouthEastCell(l)))
        neighbors.push_back(corner_ids_[l.x][l.y + 1]);

    // West
    if (IsTraversable(NorthWestCell(l)) || IsTraversable(SouthWestCell(l)))
        neighbors.push_back(corner_ids_[l.x - 1][l.y]);

    // East
    if (IsTraversable(NorthEastCell(l)) || IsTraversable(SouthEastCell(l)))
        neighbors.push_back(corner_ids_[l.x + 1][l.y]);
    return neighbors;
}
std::vector<CornerLoc> AnyAngleAlgorithm::GetNeighbors(const CornerLoc l) const
{
    // TODO: Disallow moves between diagonal obstacles? Does not seem to be used at the moment.
    std::vector<CornerLoc> neighbors;

    // NorthWest
    if (IsTraversable(NorthWestCell(l)))
        neighbors.push_back(CornerLoc(l.x - 1, l.y - 1));

    // NorthEast
    if (IsTraversable(NorthEastCell(l)))
        neighbors.push_back(CornerLoc(l.x + 1, l.y - 1));

    // SouthWest
    if (IsTraversable(SouthWestCell(l)))
        neighbors.push_back(CornerLoc(l.x - 1, l.y + 1));

    // SouthEast
    if (IsTraversable(SouthEastCell(l)))
        neighbors.push_back(CornerLoc(l.x + 1, l.y + 1));

    // North
    if (IsTraversable(NorthWestCell(l)) || IsTraversable(NorthEastCell(l)))
        neighbors.push_back(CornerLoc(l.x, l.y - 1));

    // South
    if (IsTraversable(SouthWestCell(l)) || IsTraversable(SouthEastCell(l)))
        neighbors.push_back(CornerLoc(l.x, l.y + 1));

    // West
    if (IsTraversable(NorthWestCell(l)) || IsTraversable(SouthWestCell(l)))
        neighbors.push_back(CornerLoc(l.x - 1, l.y));

    // East
    if (IsTraversable(NorthEastCell(l)) || IsTraversable(SouthEastCell(l)))
        neighbors.push_back(CornerLoc(l.x + 1, l.y));

    return neighbors;
}

void AnyAngleAlgorithm::GetRandomProblem(xyLoc &l1, xyLoc &l2) const
{
    l1 = unblocked_cells_[rand() % unblocked_cells_.size()];
    l2 = unblocked_cells_[rand() % unblocked_cells_.size()];
}

bool AnyAngleAlgorithm::LineOfSight(CornerLoc l1, CornerLoc l2)
{
    // This line of sight check uses only integer values. First it checks whether the movement along the x or the y axis is longer and moves along the longer
    // one cell by cell. dx and dy specify how many cells to move in each direction. Suppose dx is longer and we are moving along the x axis. For each
    // cell we pass in the x direction, we increase variable f by dy, which is initially 0. When f >= dx, we move along the y axis and set f -= dx. This way,
    // after dx movements along the x axis, we also move dy moves along the y axis.

    // x and y values correspond to corners, not cells.
    int x1 = l1.x; // Originate from this cell.
    int y1 = l1.y;

    int x2 = l2.x; // Move to this cell.
    int y2 = l2.y;

    int dy = l2.y - l1.y;
    int dx = l2.x - l1.x;

    if (dx > 0)   // l2 is towards East of l1.
        if (l1.type == kCornerNorthWest || l1.type == kCornerSouthWest || l2.type == kCornerNorthEast || l2.type == kCornerSouthEast)
            return false;
    if (dx < 0)   // l2 is towards West of l1.
        if (l1.type == kCornerNorthEast || l1.type == kCornerSouthEast || l2.type == kCornerNorthWest || l2.type == kCornerSouthWest)
            return false;
    if (dy > 0)   // l2 is towards South of l1.
        if (l1.type == kCornerNorthWest || l1.type == kCornerNorthEast || l2.type == kCornerSouthWest || l2.type == kCornerSouthEast)
            return false;
    if (dy < 0)   // l2 is towards North of l1.
        if (l1.type == kCornerSouthWest || l1.type == kCornerSouthEast || l2.type == kCornerNorthWest || l2.type == kCornerNorthEast)
            return false;

    int f = 0;
    int sy, sx; // Direction of movement. Value can be either 1 or -1.

    // The x and y locations correspond to corners, not cells. We might need to check different surrounding cells depending on the direction we do the
    // line of sight check. The following values are usedto determine which cell to check to see if it is unblocked.
    int x_offset, y_offset;

    if (dy < 0) {
        dy = -dy;
        sy = -1;
        y_offset = 0; // Cell is to the North
    }
    else {
        sy = 1;
        y_offset = 1; // Cell is to the South
    }

    if (dx < 0) {
        dx = -dx;
        sx = -1;
        x_offset = 0; // Cell is to the West
    }
    else {
        sx = 1;
        x_offset = 1; // Cell is to the East
    }

    if (dx >= dy) { // Move along the x axis and increment/decrement y when f >= dx.
        while (x1 != x2) {
            f = f + dy;
            if (f >= dx) {  // We are changing rows, we might need to check two cells this iteration.
                if (!IsTraversable(x1 + x_offset, y1 + y_offset))
                    return false;

                y1 = y1 + sy;
                f = f - dx;
            }

            if (f != 0) {   // If f == 0, then we are crossing the row at a corner point and we don't need to check both cells.
                if (!IsTraversable(x1 + x_offset, y1 + y_offset))
                    return false;
            }

            if (dy == 0) {  // If we are moving along a horizontal line, either the north or the south cell should be unblocked.
                if (!IsTraversable(x1 + x_offset, y1) && !IsTraversable(x1 + x_offset, y1 + 1))
                    return false;
            }

            x1 += sx;

            // TODO: Make this check faster?
            if (f == 0 && x1 != x2 && CornerAtDiagonalIntersection(x1,y1))
                 return false;
        }
    }

    else {  //if (dx < dy). Move along the y axis and increment/decrement x when f >= dy.
        while (y1 != y2) {
            f = f + dx;
            if (f >= dy) {
                if (!IsTraversable(x1 + x_offset, y1 + y_offset))
                    return false;

                x1 = x1 + sx;
                f = f - dy;
            }

            if (f != 0) {
                if (!IsTraversable(x1 + x_offset, y1 + y_offset))
                    return false;
            }

            if (dx == 0) {
                if (!IsTraversable(x1, y1 + y_offset) && !IsTraversable(x1 + 1, y1 + y_offset))
                    return false;
            }

            y1 += sy;

            // TODO: Make this check faster?
            if (f == 0 && y1 != y2 && CornerAtDiagonalIntersection(x1,y1))
                 return false;
        }
    }
    return true;
}
bool AnyAngleAlgorithm::LineOfSight(xyLocCont l1, xyLocCont l2)
{
    // Similar implementation to LineOfSight(xyLoc, xyLoc). But, this time, we might start and end with a non-zero 'f' and 'f' needs to be a float.
    // This function assumes that l1 and l2 do not lie in the interiors of obstacles.

    double dy = l2.y - l1.y;
    double dx = l2.x - l1.x;

    // Decide along which axis we iterate.
    bool iterate_over_x = fabs(dx) >= fabs(dy);

    // Swap l1 and l2 if necessary to make sure that l1.x <= l2.x if we are iterating over the x axis, or l1.y <= l2.y if we are iterating over the y axis.
    if ((iterate_over_x && dx < 0) || (!iterate_over_x && dy < 0)) {
        xyLocCont temp = l2;
        l2 = l1;
        l1 = temp;
        dy = -dy;
        dx = -dx;
    }

    if (iterate_over_x) {
        double df = fabs(dy / dx); // Each time x is increased by 1, f will be adjusted by this amount.

        // We want to iterate over integer values of x. However, x1 and/or x2 might not be integers. Therefore, we try to stretch the endpoints of the line
        // (l1,l2) to make x1 and x2 integers. However, if stretching an endpoint causes the line to intersect with the interior of a blocked cell it does not
        // originally intersect, the line-of-sight check would fail, even if l1 and l2 have line-of-sight. If this is the case, we shorten the line at the
        // problematic endpoint(s).

        // Stretch the beginning of the line.
        int x1 = floor(l1.x + EPSILON); // If l1.x is already an integer, then x1 = l1.x.
        double y1 = GetIntersectingY(l1.x, l1.y, l2.x, l2.y, x1);

        // If stretching the beginning of the line caused the line to intersect with another cell, shorten the beginning of the line instead.
        if (IntervalContainsInteger(y1, l1.y)) {
            x1 = ceil(l1.x);
            y1 = GetIntersectingY(l1.x, l1.y, l2.x, l2.y, x1);
        }

        // Stretch the end of the line.
        int x2 = ceil(l2.x - EPSILON); // If l2.x is already an integer, then x2 = l2.x.
        double y2 = GetIntersectingY(l1.x, l1.y, l2.x, l2.y, x2);

        // If stretching the end of the line caused the line to intersect with another cell, shorten the end of the line instead.
        if (IntervalContainsInteger(y2, l2.y)) {
            x2 = floor(l2.x);
            y2 = GetIntersectingY(l1.x, l1.y, l2.x, l2.y, x2);
        }

        // Now, x1 and x2 are integers. The rest is similar to line-of-sight checks with xyLocs, except for the following:
        // Determine the starting value of f and y, and direction of movement for y
        double f;
        int y;
        int sy; // The direction we update the y coordinates (-1 or +1).
        int y_offset; // Used to determine which cell to check to see if it is unblocked.

        if (dy < 0) {
            sy = -1;
            y_offset = 0; // Cell is to the North
            y = ceil(y1 - EPSILON);
        }
        else {
            sy = 1;
            y_offset = 1; // Cell is to the South
            y = floor(y1 + EPSILON);
        }
        f = fabs(y1 - (double) y);

        while (x1 != x2) {
            f = f + df;
            if (f >= 1) {   // We are changing rows, we might need to check two cells this iteration.
                if (!IsTraversable(x1 + 1, y + y_offset))
                    return false;

                y = y + sy;
                f = f - 1;
            }

            if (f > EPSILON) {  // If f == 0, then we are crossing the row at a corner point and we don't need to check both cells.
                if (!IsTraversable(x1 + 1, y + y_offset))
                    return false;
            }

            if (fabs(dy) < EPSILON) {   // If we are moving along a horizontal line, either the north or the south cell should be unblocked.
                if (!IsTraversable(x1 + 1, y) && !IsTraversable(x1 + 1, y + 1))
                    return false;
            }

            x1 += 1;
        }
    }

    else {  //if we are iterating over y
        // Same as the case with x. Except we swap x and y.

        double df = fabs(dx / dy); // Each time y is increased by 1, f will be adjusted by this amount

        // Stretch the beginning of the line.
        int y1 = floor(l1.y + EPSILON); // If l1.y is already an integer, then y1 = l1.y.
        double x1 = GetIntersectingX(l1.x, l1.y, l2.x, l2.y, y1);

        // If stretching the beginning of the line caused the line to intersect with another cell, shorten the beginning of the line instead.
        if (IntervalContainsInteger(x1, l1.x)) {
            y1 = ceil(l1.y);
            x1 = GetIntersectingX(l1.x, l1.y, l2.x, l2.y, y1);
        }

        // Stretch the end of the line.
        int y2 = ceil(l2.y - EPSILON); // If l2.y is already an integer, then y2 = l2.y.
        double x2 = GetIntersectingX(l1.x, l1.y, l2.x, l2.y, y2);

        // If stretching the end of the line caused the line to intersect with another cell, shorten the end of the line instead.
        if (IntervalContainsInteger(x2, l2.x)) {
            y2 = floor(l2.y);
            x2 = GetIntersectingX(l1.x, l1.y, l2.x, l2.y, y2);
        }

        // Determine the starting value of f and x, and direction of movement for x.
        double f;
        int x;
        int sx; // The direction we update the x coordinates (-1 or +1).
        int x_offset; // Used to determine which cell to check to see if it is unblocked.

        if (dx < 0) {
            sx = -1;
            x_offset = 0; // Cell is to the North
            x = ceil(x1 - EPSILON);
        }
        else {
            sx = 1;
            x_offset = 1; // Cell is to the South
            x = floor(x1 + EPSILON);
        }
        f = fabs(x1 - (double) x);

        while (y1 != y2) {
            f = f + df;
            if (f >= 1) {   // We are changing columns, we might need to check two cells this iteration.
                if (!IsTraversable(x + x_offset, y1 + 1))
                    return false;

                x = x + sx;
                f = f - 1;
            }

            if (f > EPSILON) {  // If f == 0, then we are crossing the column at a corner point and we don't need to check both cells.
                if (!IsTraversable(x + x_offset, y1 + 1))
                    return false;
            }

            if (fabs(dx) < EPSILON) {   // If we are moving along a vertical line, either the north or the south cell should be unblocked.
                if (!IsTraversable(x, y1 + 1) && !IsTraversable(x + 1, y1 + 1))
                    return false;
            }

            y1 += 1;
        }
    }
    return true;
}

cost AnyAngleAlgorithm::SmoothPath(const std::vector<CornerLoc> &path, std::vector<CornerLoc> &smoothed_path)
{
    smoothed_path.clear();

    if (path.empty())
        return INFINITE_COST;

    // Add the start.
    smoothed_path.push_back(path[0]);

    // Go over all locations on the original path, in order.
    for (unsigned int i = 1; i < path.size(); i++) {
        if (!LineOfSight(smoothed_path.back(), path[i])) // If there is no line of sight to the last location that is added to the smoothed path..
        {
            smoothed_path.push_back(path[i - 1]); // ..add the i-1st location on the original path to the smoothed path.
        }
    }

    // Add the goal.
    if (!(smoothed_path.back().x == path.back().x && smoothed_path.back().y == path.back().y))
        smoothed_path.push_back(path.back());

    // Compute path cost.
    cost c = 0;
    for (unsigned int i = 1; i < smoothed_path.size(); i++)
        c += EuclideanDistance(smoothed_path[i-1], smoothed_path[i]);

    return c;
}
cost AnyAngleAlgorithm::SmoothPath(const std::vector<xyLocCont> & path, std::vector<xyLocCont> &smoothed_path)
{
    smoothed_path.clear();

    if (path.empty())
        return INFINITE_COST;

    // Add the start.
    smoothed_path.push_back(path[0]);

    // Go over all locations on the original path, in order.
    for (unsigned int i = 1; i < path.size(); i++) {
        if (!LineOfSight(smoothed_path.back(), path[i])) // If there is no line of sight to the last location that is added to the smoothed path..
        {
            smoothed_path.push_back(path[i - 1]); // ..add the i-1st location on the original path to the smoothed path.
        }
    }

    // Add the goal.
    if (fabs(smoothed_path.back().x - path.back().x) > EPSILON || fabs(smoothed_path.back().y - path.back().y) > EPSILON)
        smoothed_path.push_back(path.back());

    // Compute path cost.
    cost c = 0;
    for (unsigned int i = 1; i < smoothed_path.size(); i++)
        c += EuclideanDistance(smoothed_path[i-1], smoothed_path[i]);

    return c;
}
/*End AnyAngleAlgorithm.cpp*/
/*Start ThetaStar.h*/
#define A_STAR_EUC 0
#define A_STAR_OCT 1
#define THETA_STAR 2
#define LAZY_THETA_STAR 3

class ThetaStar : public AnyAngleAlgorithm
{
public:
	ThetaStar(std::vector<bool> &bits, int width, int height, int search_method = THETA_STAR);
    ~ThetaStar();

    const std::string GetName() const;
	cost FindCornerLocPath(xyLoc from, xyLoc to, std::vector<CornerLoc> &path);

private:
	// Use either A* (with Euclidean or Octile distance), Theta* or Lazy Theta*.
	int search_method_;

    // The search counter. Any state whose generated value is equal to this value is considered to be generated. Resets every 50,000 searches.
    uint16_t search_;

	// Search info
	struct Corner{
		Corner(){}

		std::vector<cornerId> neighbors;	// Neighbors on the grid (max 8).
		std::vector<cost> neighbor_dist;	// neighbor_dist[i] is the distance from this corner to neighbors[i].
		uint16_t generated;	// The id of the last search in which this corner has been generated.
		cornerId parent;	// Parent of the corner.
		cost g_val;			// Initialized to infinity when generated.
		cost h_val;			// Initialized to the heuristic distance to the goal when generated.
		char list;			// Initially NO_LIST, can be changed to OPEN_LIST or CLOSED_LIST.

		int heap_index;		// The location of the corner on the open list
	};
	std::vector<Corner> corners_;

	// Priority queue
	struct HeapElement
	{
		HeapElement(cornerId _id, cost _g_val, cost _f_val) : id(_id), g_val(_g_val), f_val(_f_val) {}
		cornerId id;
		cost g_val;	// Used for tie-breaking
		cost f_val;	// Main key

		bool operator<(const HeapElement& rhs) const
		{
		    //return fVal < rhs.fVal;	// no-tie break
		  	return (f_val + EPSILON < rhs.f_val) ? true : ((rhs.f_val + EPSILON < f_val) ? false : g_val > rhs.g_val);	// tie-break towards larger g-values
		}
	};
	std::vector<HeapElement> open_list_;

    void ThetaStarInitialize(const int search_method);

    // Lazy Theta* assumes that there is always line-of-sight from the parent of an expanded state to a successor state.
    // When expanding a state, it checks if this assumption was true. If not, it updates the g-value of the state and puts it back in the open list.
    void ValidateParent(const cornerId s, const cornerId goal);

    // Main search methods.
    cost AStarSearch(const xyLoc from, const xyLoc to, std::vector<CornerLoc> & path);
    cost ThetaStarSearch(const xyLoc from, const xyLoc to, std::vector<CornerLoc> & path);
    cost LazyThetaStarSearch(const xyLoc from, const xyLoc to, std::vector<CornerLoc> & path);

    // Generates a corner as a state if its 'generated' value is lower than 'search_counter_'. Each corner can be generated once per search.
    void GenerateState(const cornerId s, const cornerId goal);

    // Set the search counter and all generated values to 0
    void ResetSearch();

    // Heap operations.
	void AddToOpen(const cornerId id);
	HeapElement GetMin() const {return open_list_.front();}
	void PopMin();	// Remove the corner with the minimum f-value from the open list and move it to the closed list
	void PercolateUp(int index);
	void PercolateDown(int index);
};
/*End ThetaStar.h*/
/*Start ThetaStar.cpp*/
ThetaStar::ThetaStar(std::vector<bool> &bits, int _width, int _height, int search_method) :
    AnyAngleAlgorithm::AnyAngleAlgorithm(bits, _width, _height)
{
    ThetaStarInitialize(search_method);
}
void ThetaStar::ThetaStarInitialize(const int search_method)
{
    int num_edges = 0;
    search_method_ = search_method;
    // Generate the edges between cells, for easy look-up.
    corners_.resize(corner_locations_.size());
    for (cornerId i = 0; i < corner_locations_.size(); i++) {
        corners_[i].neighbors = GetNeighbors(i);
        for (unsigned int j = 0; j < corners_[i].neighbors.size(); j++)
            corners_[i].neighbor_dist.push_back(EuclideanDistance(i, corners_[i].neighbors[j]));
        num_edges += corners_[i].neighbors.size();
    }
    std::cout<<"Number of edges: "<<num_edges<<std::endl;

    // Reset the generated values.
    ResetSearch();
    open_list_.reserve(10000);
}
ThetaStar::~ThetaStar()
{
}

const std::string ThetaStar::GetName() const
{
    switch (search_method_) {

    case A_STAR_EUC:
        return "A_EUC";

    case A_STAR_OCT:
        return "A_OCT";

    case THETA_STAR:
        return "T";

    case LAZY_THETA_STAR:
        return "L";

    default:
        return "";
    }
}

cost ThetaStar::FindCornerLocPath(xyLoc from, xyLoc to, std::vector<CornerLoc> &path)
{
    switch (search_method_) {

    case A_STAR_EUC:
        UseEuclideanDistance();
        return AStarSearch(from, to, path);

    case A_STAR_OCT:
        UseOctileDistance();
        return AStarSearch(from, to, path);

    case THETA_STAR:
        UseEuclideanDistance();
        return ThetaStarSearch(from, to, path);

    case LAZY_THETA_STAR:
        UseEuclideanDistance();
        return LazyThetaStarSearch(from, to, path);

    default:
        return INFINITE_COST;
    }
}

void ThetaStar::ValidateParent(const cornerId s, const cornerId goal)
{
    // Lazy Theta* assumes that there is always line-of-sight from the parent of an expanded state to a successor state.
    // When expanding a state, check if this is true.
    if (!LineOfSight(corners_[s].parent, s)) {

        // Since the previous parent is invalid, set g-value to infinity.
        corners_[s].g_val = INFINITE_COST;

        // Go over potential parents and update its parent to the parent that yields the lowest g-value for s.
        for (unsigned int i = 0; i < corners_[s].neighbors.size(); i++) {
            cornerId new_parent = corners_[s].neighbors[i];
            GenerateState(new_parent, goal);
            if (corners_[new_parent].list == CLOSED_LIST) {
                cost new_g_val = corners_[new_parent].g_val + corners_[s].neighbor_dist[i];
                if (new_g_val < corners_[s].g_val) {
                    corners_[s].g_val = new_g_val;
                    corners_[s].parent = new_parent;
                }
            }
        }
    }
}

cost ThetaStar::AStarSearch(const xyLoc from, const xyLoc to, std::vector<CornerLoc> & path)
{
    if (search_ >= MAX_SEARCH)
        ResetSearch();

    // Initialize the search.
    search_++;
    path.clear();

    cornerId start = ToCornerId(from);
    cornerId goal = ToCornerId(to);

    open_list_.clear();
    GenerateState(start, goal);
    GenerateState(goal, goal);

    corners_[start].g_val = 0;
    AddToOpen(start);

    while (!open_list_.empty() && corners_[goal].g_val > GetMin().f_val + EPSILON) {
        cornerId curr = GetMin().id;
        PopMin();
        for (unsigned int i = 0; i < corners_[curr].neighbors.size(); i++) {
            cornerId succ = corners_[curr].neighbors[i];
            GenerateState(succ, goal);

            if (corners_[succ].list != CLOSED_LIST) {
                cost new_g_val = corners_[curr].g_val + corners_[curr].neighbor_dist[i];

                if (new_g_val + EPSILON < corners_[succ].g_val) {
                    corners_[succ].g_val = new_g_val;
                    corners_[succ].parent = curr;
                    AddToOpen(succ);
                }
            }
        }
    }

    // Extract the path.
    if (corners_[goal].g_val < INFINITE_COST) {
        cornerId curr = goal;
        while (curr != start) {
            path.push_back(corner_locations_[curr]);
            curr = corners_[curr].parent;
        }

        path.push_back(corner_locations_[curr]);
        std::reverse(path.begin(), path.end());
    }

#ifdef ANY_ANGLE_ASSERTIONS
    // Make sure that the heap property is satisfied for the open list
    for (int i = 0; i < open_list_.size(); i++) {
        int child1 = (i << 1) + 1;
        int child2 = (i << 1) + 2;

        if (child2 < open_list_.size()) {
            assert(open_list_[i].f_val < open_list_[child1].f_val + 100*EPSILON);
            assert(open_list_[i].f_val < open_list_[child2].f_val + 100*EPSILON);
        }
    }
#endif

    return corners_[goal].g_val;
}
cost ThetaStar::ThetaStarSearch(const xyLoc from, const xyLoc to, std::vector<CornerLoc> & path)
{
    if (search_ >= MAX_SEARCH)
        ResetSearch();

    // Initialize the search.
    search_++;
    path.clear();

    cornerId start = ToCornerId(from);
    cornerId goal = ToCornerId(to);

    open_list_.clear();
    GenerateState(start, goal);
    GenerateState(goal, goal);
    corners_[start].g_val = 0;

    // Set 'start's parent as itself. When 'start' is expanded for the first time, the grandparent of 'start's successor will be 'start' as well.
    corners_[start].parent = start;

    AddToOpen(start);

    while (!open_list_.empty() && corners_[goal].g_val > GetMin().f_val + EPSILON) {
        cornerId curr = GetMin().id;
        PopMin();
        for (unsigned int i = 0; i < corners_[curr].neighbors.size(); i++) {
            cornerId succ = corners_[curr].neighbors[i];
            GenerateState(succ, goal);

            if (corners_[succ].list != CLOSED_LIST) {
                cost new_g_val;
                cornerId new_parent;

                // First, check if there is LOS from the grandparent.
                if (LineOfSight(corners_[curr].parent, succ)) {
                    new_parent = corners_[curr].parent;
                    new_g_val = corners_[new_parent].g_val + EuclideanDistance(new_parent, succ);
                }
                else {
                    new_parent = curr;
                    new_g_val = corners_[new_parent].g_val + corners_[curr].neighbor_dist[i];
                }

                if (new_g_val + EPSILON < corners_[succ].g_val) {
                    corners_[succ].g_val = new_g_val;
                    corners_[succ].parent = new_parent;
                    AddToOpen(succ);
                }
            }
        }
    }

    // Extract the path.
    if (corners_[goal].g_val < INFINITE_COST)
    {
        cornerId curr = goal;
        while (curr != start) {
            path.push_back(corner_locations_[curr]);
            curr = corners_[curr].parent;
        }

        path.push_back(corner_locations_[curr]);
        std::reverse(path.begin(), path.end());
    }
    return corners_[goal].g_val;
}
cost ThetaStar::LazyThetaStarSearch(const xyLoc from, const xyLoc to, std::vector<CornerLoc> & path)
{
    if (search_ >= MAX_SEARCH)
        ResetSearch();

    // Initialize the search.
    search_++;
    path.clear();

    cornerId start = ToCornerId(from);
    cornerId goal = ToCornerId(to);

    open_list_.clear();
    GenerateState(start, goal);
    GenerateState(goal, goal);
    corners_[start].g_val = 0;

    // Set 'start's parent as itself. When 'start' is expanded for the first time, the grandparent of 'start's successor will be 'start' as well.
    corners_[start].parent = start;

    AddToOpen(start);

    while (!open_list_.empty() && corners_[goal].g_val > GetMin().f_val + EPSILON) {
        cornerId curr = GetMin().id;
        PopMin();
        // First, check if we have LOS from the parent and update the parent and g-value of curr if necessary.
        ValidateParent(curr, goal);

        // Assume there is LOS from the parent of the state being expanded to the successor
        cornerId new_parent = corners_[curr].parent;

        for (unsigned int i = 0; i < corners_[curr].neighbors.size(); i++) {
            cornerId succ = corners_[curr].neighbors[i];
            GenerateState(succ, goal);

            if (corners_[succ].list != CLOSED_LIST) {
                cost new_g_val = corners_[new_parent].g_val + EuclideanDistance(new_parent, succ);
                if (new_g_val + EPSILON < corners_[succ].g_val) {
                    corners_[succ].g_val = new_g_val;
                    corners_[succ].parent = new_parent;
                    AddToOpen(succ);
                }
            }
        }
    }

    // Extract the path.
    if (corners_[goal].g_val < INFINITE_COST)
    {
        ValidateParent(goal, goal);
        cornerId curr = goal;
        while (curr != start) {
            path.push_back(corner_locations_[curr]);
            curr = corners_[curr].parent;
        }

        path.push_back(corner_locations_[curr]);
        std::reverse(path.begin(), path.end());
    }

#ifdef ANY_ANGLE_ASSERTIONS
    // Make sure that the heap property is satisfied for the open list
    for (int i = 0; i < open_list_.size(); i++) {
        int child1 = (i << 1) + 1;
        int child2 = (i << 1) + 2;

        if (child2 < open_list_.size()) {
            assert(open_list_[i].f_val < open_list_[child1].f_val + 100*EPSILON);
            assert(open_list_[i].f_val < open_list_[child2].f_val + 100*EPSILON);
        }
    }
#endif

    return corners_[goal].g_val;
}

void ThetaStar::GenerateState(cornerId s, cornerId goal)
{
    if (corners_[s].generated < search_) {
        corners_[s].generated = search_;
        corners_[s].h_val = HeuristicDistance(s, goal);
        corners_[s].g_val = INFINITE_COST;
        corners_[s].list = NO_LIST;
    }
}
void ThetaStar::ResetSearch()
{
    // Set last search and generated values to 0, so that when the search is incremented, all states are un-generated
    search_ = 0;
    for (cornerId i = 0; i < corners_.size(); i++)
        corners_[i].generated = 0;
}

// Heap operations.
void ThetaStar::AddToOpen(cornerId id)
{
    // If it is already in the open list, update its location.
    if (corners_[id].list == OPEN_LIST) {
        int i = corners_[id].heap_index;
        open_list_[i].g_val = corners_[id].g_val;
        open_list_[i].f_val = corners_[id].g_val + corners_[id].h_val;
        PercolateUp(i);
    }
    // Otherwise, add it to the open list
    else {
        corners_[id].list = OPEN_LIST;
        corners_[id].heap_index = open_list_.size();
        open_list_.push_back(HeapElement(id, corners_[id].g_val, corners_[id].g_val + corners_[id].h_val));
        PercolateUp(open_list_.size() - 1);
    }
}
void ThetaStar::PopMin()
{
    corners_[open_list_[0].id].list = CLOSED_LIST;
    open_list_[0] = open_list_.back();
    corners_[open_list_[0].id].heap_index = 0;
    open_list_.pop_back();
    PercolateDown(0);
}
void ThetaStar::PercolateUp(int index)
{
    HeapElement elem = open_list_[index];
    int parent = (index - 1) >> 1;

    while (index > 0 && elem < open_list_[parent]) {
        open_list_[index] = open_list_[parent];
        corners_[open_list_[index].id].heap_index = index;
        index = parent;
        parent = (index - 1) >> 1;
    }

    open_list_[index] = elem;
    corners_[elem.id].heap_index = index;
}
void ThetaStar::PercolateDown(int index)
{
    HeapElement elem = open_list_[index];
    int maxSize = open_list_.size();

    while (true) {
        int child1 = (index << 1) + 1;
        if (child1 >= maxSize)
            break;

        int child2 = child1 + 1;

        // If the first child has smaller key
        if (child2 == maxSize || open_list_[child1] < open_list_[child2]) {
            if (open_list_[child1] < elem) {
                open_list_[index] = open_list_[child1];
                corners_[open_list_[index].id].heap_index = index;
                index = child1;
            }
            else
                break;
        }

        else if (open_list_[child2] < elem) {
            open_list_[index] = open_list_[child2];
            corners_[open_list_[index].id].heap_index = index;
            index = child2;
        }
        else
            break;
    }

    open_list_[index] = elem;
    corners_[elem.id].heap_index = index;
}
/*End ThetaStar.cpp*/
//End T* dependencies

using namespace client;

void LoadMap(char mapple[4][4], std::vector<bool> &map, int &width, int &height)
{
	console.log("Hello Third Point FiveOne");
	map.resize(height*width);
    console.log("Hello Third Point FiveFifth");
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            console.log("Hello Third Point FiveSixth");
            char c = mapple[y][x];
            map[y*width+x] = (c == '.' || c == 'G' || c == 'S');
            console.log("Hello Third Point FiveEighth");
        }
    }
    console.log("Hello Third Point FiveNinth");
}

void loadCallback() {
     // Process the arguments
	console.log("Hello Second");
    char mapname[255];
	char scenname[255];
	std::vector<bool> mapData;
	int width, height;
	console.log("Hello Third");
	sprintf(mapname, "%s.map", "test");
    console.log("Hello Third Point Five");
	LoadMap([['@','@','@','@'],['@','.','.','@'],['@','.','.','@'],['@','@','@','@']], mapData, width, height);	// Read the map
	console.log("Hello Fourth");
	//Timer t;
	//t.StartTimer();
	double totalTime = 0.0;
	
    // A* with Euclidean distance heuristic
	AnyAngleAlgorithm* thetaStar = new ThetaStar(mapData, width, height, A_STAR_EUC);
    console.log("Hello Fifth");
    // Solve the instances
	xyLoc s, g;
    s.x = 0;
    s.y = 0;
    g.x = 1;
    g.y = 1;
    console.log("Hello Sixth");
    thetaStar->FindPath(s,g);
	console.log("Hello Seventh");
	//totalTime += t.EndTimer();
    
    // Mark the end of the experiment
	console.log("Total time spent: ");
    console.log(totalTime*1000);
    console.log("ms.");
	
	// Clean up
    delete thetaStar;
 }

void webMain()
{
    console.log("Hello First");
    document.addEventListener("DOMContentLoaded",cheerp::Callback(loadCallback));
}


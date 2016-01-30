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

#ifndef ANY_ANGLE_ALGORITHM_H
#define ANY_ANGLE_ALGORITHM_H

#include <deque>
#include <queue>
#include <vector>
//#include <ext/hash_map>
#include "Timer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <math.h>


#include "AnyAngleDefinitions.h"

#ifdef ANY_ANGLE_STATISTICS
#include "AnyAngleStatistics.h"
#endif

#ifdef ANY_ANGLE_ASSERTIONS
#include <cassert>
#endif

// Vertices are placed at the corners of cells.
// Given a map of size n x m (n x m cells, n+1 x m+1 corners):
// Top left cell is (0,0), top left corner is (0,0), and corner (x,y) is located at the top left corner of cell (x,y).
//
// We add a frame of obstacles around the map but don't add any more corners, resulting in n+2 x m+2 cells, n+1 x m+1 corners.
//
// Now, corner x,y is located at the bottom right corner of cell x,y, and every corner is surrounded by cells so we don't have to do any out-of-map-bounds
// checks.
struct xyLoc {
public:
    xyLoc() {}
    xyLoc(uint16_t _x, uint16_t _y) :x(_x), y(_y) {}
    uint16_t x;
    uint16_t y;
};

class AnyAngleAlgorithm {

public:

#ifdef ANY_ANGLE_RUNNING_IN_HOG
    //AnyAngleAlgorithm(const MapEnvironment *env);                // Constructs the class using a map given as a MapEnvironment in HOG
#endif
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

#ifdef ANY_ANGLE_STATISTICS
    // Generates two AnyAngleStatistics instances, for statistics with and without smoothing.
    virtual void SetupStatistics();

    // Opens up output files (algorithm_id-mapname) for writing the statistics.
    virtual void SetStatisticsFiles(const std::string mapname);

    // Prints all the statistics kept by the AnyAngleStatistics classes to the output files.
    virtual void PrintStatistics();

    // Called by PrintStatistics in case the derived class has additional statistics to report.
    virtual void PrintAdditionalStatistics(AnyAngleStatistics* stats) {};
#endif


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

#ifdef DISALLOW_MOVES_BETWEEN_DIAGONAL_OBSTACLES
	void ComputeCornerNeighbors();
	std::vector<cornerId> GetCornerNeighbors(const cornerId c) const;
	cornerId GetCornerNeighbor(const cornerId c, const direction d) const {return corner_neighbors_[c][d];};
#endif

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

#ifdef ANY_ANGLE_STATISTICS
    // Statistics variables. Number of line of sight checks, elapsed time, and heading changes are maintained by this class.
    // Any child class should maintain search related statistics by incrementing the relevant variables.
    int num_expansions_, num_generated_, num_percolations_, num_los_checks_;
    int num_heading_changes_, num_freespace_heading_changes_, num_taut_corner_heading_changes_, num_non_taut_corner_heading_changes_;

    Timer timer_;
    double elapsed_time_;

    AnyAngleStatistics *statistics_, *statistics_with_smoothing_;

    // Generates statistics for the paths and (optionally) validates the path by checking if it intersects with any obstacles.
    // Validation performs line-of-sight checks, but these are not reported in the statistics.
    cost EvaluatePath(const std::vector<CornerLoc> & path, const bool validate_path = true);
    cost EvaluatePath(const std::vector<xyLocCont> & path, const bool validate_path = true);

    // Normalizes the degree of an angle to the range [0, 360)
    double NormalizeAngle(const double theta) const;

    // Returns the degree of the angle [0, 360) between the x-axis and the vector (x1,y1)->(x2,y2).
    double GetAngle(const double x1, const double y1, const double x2, const double y2) const;

    // Returns true if the path <(x1, y1), (x2, y2), (x3, y3)> is taut, where (x2, y2) is a corner point.
    bool IsTautCornerTurn(const double x1, const double y1, const int x2, const int y2, const double x3, const double y3) const;

    // Resets all the statistics variables (should be called before each search)
    void StartStatistics(const xyLoc from, const xyLoc to);

    // Reports all the statistics for the current search to the AnyAngleStatistics classes.
    void ReportStatistics(const std::vector<CornerLoc> & path, AnyAngleStatistics* stats, const bool validate_path = true);
    void ReportStatistics(const std::vector<xyLocCont> & path, AnyAngleStatistics* stats, const bool validate_path = true);
#endif


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

#endif

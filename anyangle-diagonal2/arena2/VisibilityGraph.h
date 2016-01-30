#ifndef VISIBILITY_GRAPH_H
#define VISIBILITY_GRAPH_H
#include "AnyAngleAlgorithm.h"

class VisibilityGraph : public AnyAngleAlgorithm
{
public:
	VisibilityGraph(std::vector<bool> &bits, int width, int height);
	~VisibilityGraph();

#ifdef ANY_ANGLE_RUNNING_IN_HOG
	VisibilityGraph(MapEnvironment *env);
	void OpenGLDraw(const MapEnvironment *env);
#endif

	void SetOutputFiles(const char * mapname);
	void SearchStatistics();
	void Solve(xyLoc l1, xyLoc l2);

private:

	void VisibilityGraphInitialize();

	void GenerateState(cornerId s, cornerId goal);	// Initialize a state for a search if it has not been initialized before for this search
	void ResetSearch();	// Set the search counter and all generated values to 0

	cost VisibilityGraphSearch(xyLoc l1, xyLoc l2, std::vector<xyLoc> & thePath);

	// Search info
	struct corner{
		corner(){}

		std::vector<cornerId> neighbors;	// neighbors in the grid (max 8)
		std::vector<cost> neighborDist;		// neighborDist[i] is the distance from this corner to neighbors[i]
		uint16_t generated;	// The id of the last search that this corner has been initialized in
		cornerId parent;	// Parent of the corner
		cost gVal;			// Initialized to infinity when generated
		cost hVal;			// Initialized to the heuristic distance to the goal when generated
		char list;			// Initially NO_LIST, can be changed to OPEN_LIST or CLOSED_LIST

		int heapIndex;		// The location of the corner on the open list
	};
	std::vector<corner> corners;	// For each corner, keeps the search related information

	// One AnyAngleStatistics class for each algorithm
	AnyAngleStatistics *VisibilityGraphStats;

	// Display variables
	std::vector<xyLoc> VisibilityGraphPath;

	// Priority queue
	struct heapElement
	{
		heapElement(cornerId _id, cost _gVal, cost _fVal) : id(_id), gVal(_gVal), fVal(_fVal) {}
		cornerId id;
		cost gVal;	// Used for tie-breaking
		cost fVal;	// Main key

		bool operator<(const heapElement& rhs) const
		{
		    //return fVal < rhs.fVal;	// no-tie break
		  	return (fVal + EPSILON < rhs.fVal) ? true : ((rhs.fVal + EPSILON < fVal) ? false : gVal > rhs.gVal);	// tie-break towards larger g-values
		}
	};
	std::vector<heapElement> theHeap;

	void AddToOpen(cornerId id);	// Add a corner to the open list or update its location on the open list
	heapElement GetMin(){return theHeap.front();}
	void PopMin();	// Remove the corner with the minimum f-value from the open list and move it to the closed list
	void PercolateUp(int index);
	void PercolateDown(int index);
};



#endif

#include <cheerp/client.h>
#include <cheerp/clientlib.h>
#include <stdio.h>
#include <stdint.h>
#include <numeric>
#include <algorithm>
//#include "Timer.h"
#include "ThetaStar.h"

using namespace client;

void LoadMap(const char *fname, std::vector<bool> &map, int &width, int &height)
{
	FILE *f;
	f = fopen(fname, "r");
	if (f)
    {
		fscanf(f, "type octile\nheight %d\nwidth %d\nmap\n", &height, &width);
		map.resize(height*width);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				char c;
				do {
					fscanf(f, "%c", &c);
				} while (isspace(c));
				map[y*width+x] = (c == '.' || c == 'G' || c == 'S');
			}
		}
		fclose(f);
    }
}

void loadCallback() {
     // Process the arguments
	console.log("Hello");
    char mapname[255];
	char scenname[255];
	std::vector<bool> mapData;
	int width, height;
	
	sprintf(mapname, "%s.map", "test");
	LoadMap(mapname, mapData, width, height);	// Read the map
	
	//Timer t;
	//t.StartTimer();
	double totalTime = 0.0;
	
    // A* with Euclidean distance heuristic
	AnyAngleAlgorithm* thetaStar = new ThetaStar(mapData, width, height, A_STAR_EUC);
    
    //AnyAngleStatistics
    thetaStar-> SetStatisticsFiles("test");
    
    // Solve the instances
	xyLoc s, g;
    s.x = 0;
    s.y = 0;
    g.x = 1;
    g.y = 1;

    thetaStar->FindPath(s,g);
	
	//totalTime += t.EndTimer();
    
    //Print Statistics
    thetaStar->PrintStatistics(); 
    
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


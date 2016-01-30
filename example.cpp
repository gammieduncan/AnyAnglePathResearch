#include <cheerp/client.h>
#include <cheerp/clientlib.h>
#include <stdio.h>
#include <stdint.h>
#include <numeric>
#include <algorithm>
#include "ScenarioLoader.h"
#include "Timer.h"
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
				//printf("%c", c);
			}
			//printf("\n");
		}
		fclose(f);
    }
}

void loadCallback() {
     // Process the arguments
	char mapname[255];
	char scenname[255];
	std::vector<bool> mapData;
	int width, height;
	
	sprintf(mapname, "%s.map", "arena2");
	sprintf(scenname, "%s.map.scen", "arena2");
	LoadMap(mapname, mapData, width, height);	// Read the map
	ScenarioLoader scen(scenname);				// Read the scenario file
	
	Timer t;
	t.StartTimer();
	double totalTime = 0.0;
	
	// Prepare the algorithms
	std::vector<AnyAngleAlgorithm*> algorithms;
    // A* with Euclidean distance heuristic
	algorithms.push_back(new ThetaStar(mapData, width, height, A_STAR_EUC));
    
    //AnyAngleStatistics
    for (int i = 0; i < algorithms.size(); i++)
		algorithms[i]-> SetStatisticsFiles("arena2");
    
    // Solve the instances
	for (int x = 0; x < scen.GetNumExperiments(); x++)
    {
			xyLoc s, g;
			s.x = scen.GetNthExperiment(x).GetStartX();
			s.y = scen.GetNthExperiment(x).GetStartY();
			g.x = scen.GetNthExperiment(x).GetGoalX();
			g.y = scen.GetNthExperiment(x).GetGoalY();

			for (int i = 0; i < algorithms.size(); i++)
				algorithms[i]-> FindPath(s,g);
    }
	
	totalTime += t.EndTimer();
    
    //Print Statistics
    for (int i = 0; i < algorithms.size(); i++)
		algorithms[i]->PrintStatistics(); 
    
    // Mark the end of the experiment
	std::cout<<"Total time spent: "<<totalTime*1000<<"ms."<<std::endl<<std::endl;
	
	// Clean up
	for (int i = 0; i < algorithms.size(); i++)
		delete algorithms[i];
 }

void webMain()
{
    document.addEventListener("DOMContentLoaded",cheerp::Callback(loadCallback));
}


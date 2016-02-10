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

#include <stdio.h>
#include <stdint.h>
#include <numeric>
#include <algorithm>
#include "ScenarioLoader.h"
#include "Timer.h"

#include "ThetaStar.h"
#include "BlockAStar.h"
#include "FieldAStar.h"
#include "SubgoalAA.h"
#include "ANYA.h"

// These should be defined in the makefile

//#define EXPERIMENT_A_EUC
//#define EXPERIMENT_A_OCT
//#define EXPERIMENT_T
//#define EXPERIMENT_L
//#define EXPERIMENT_F
//#define EXPERIMENT_B
//#define EXPERIMENT_SUB_1_A
//#define EXPERIMENT_SUB_1_T
//#define EXPERIMENT_SUB_2_A
//#define EXPERIMENT_SUB_2_T
//#define EXPERIMENT_SUB_10000_A
//#define EXPERIMENT_SUB_10000_T
//#define EXPERIMENT_ANYA

void LoadMap(const char *fname, std::vector<bool> &map, int &w, int &h);

int main(int argc, char **argv)
{
#ifndef ANY_ANGLE_STATISTICS
	std::cout<<"ANY_ANGLE_STATISTICS undefined. No point in running benchmarks!"<<std::endl;
	return 1;
#endif

	// Process the arguments
	char mapname[255];
	std::vector<bool> mapData;
	int width, height;
	
	sprintf(mapname, "%s.map", argv[1]);
	LoadMap(mapname, mapData, width, height);	// Read the map
	
	Timer t;
	t.StartTimer();
	double totalTime = 0.0;
	
	totalTime += t.EndTimer();

    ThetaStar* tss = new ThetaStar(mapData, width, height, THETA_STAR);
    
    xyLoc start(100, 167);
    xyLoc goal(150, 51);
    std::vector<CornerLoc> path;
    cost c;
    c = tss->FindCornerLocPath(start, goal, path);
    
    // Mark the end of the experiment
	std::cout<<"Total time spent: "<<totalTime*1000<<"ms."<<std::endl<<std::endl;
    std::cout<<"Length: "<<c<<std::endl;
    for(int c = 0; c < path.size(); c++) {
        std::cout<<path[c].x<<", "<<path[c].y<<std::endl;
    }
	
	delete tss;

	return 0;
}

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

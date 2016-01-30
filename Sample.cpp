/*
 * $Id: sample.cpp,v 1.23 2006/11/01 23:33:56 nathanst Exp $
 *
 *  sample.cpp
 *  hog
 *
 *  Created by Nathan Sturtevant on 5/31/05.
 *  Copyright 2005 Nathan Sturtevant, University of Alberta. All rights reserved.
 *
 * This file is part of HOG.
 *
 * HOG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * HOG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with HOG; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "Common.h"
#include "Sample.h"
#include "UnitSimulation.h"
#include "EpisodicSimulation.h"
#include "Map2DEnvironment.h"
#include "RandomUnits.h"
#include "AStar.h"
#include "TemplateAStar.h"
#include "GraphEnvironment.h"
#include "MapSectorAbstraction.h"
#include "StatUtil.h"
//#include "SubgoalAnalyzer.h"
#include "SubgoalAA.h"
#include "ThetaStar.h"
#include "BlockAStar.h"
#include "FieldAStar.h"
#include "ANYA.h"

#include "AnyAngleHOG.h"

bool mouseTracking = false;
int px1, py1, px2, py2;
int mazeSize = 64;
int gStepsPerFrame = 4;

unsigned int ssCount = 0;


xyLoc from, to;

//SubgoalAnalyzer* analyzer;
//SubgoalAA* subgoalAA;
//ThetaStar* thetaStar;
//BlockAStar* blockAStar;
//FieldAStar* fieldAStar;
//ANYA* anya;

AnyAngleHOG* aaAlgorithms;

std::vector<UnitMapSimulation *> unitSims;

int main(int argc, char* argv[])
{
	InstallHandlers();
	RunHOGGUI(argc, argv);

	from = xyLoc(1,1);
	to = xyLoc(1,1);
}

//ContractionHierarchy *ch;
/**
 * This function is used to allocate the unit simulated that you want to run.
 * Any parameters or other experimental setup can be done at this time.
 */
void CreateSimulation(int id)
{
	Map *map;
	if (gDefaultMap[0] == 0)
	{
		srandom(10);
		map = new Map(mazeSize, mazeSize);
		MakeMaze(map, 2);
		//map->Scale(mazeSize, mazeSize);
	}
	else {
		map = new Map(gDefaultMap);
//		map->Scale(512, 512);
	}
	map->SetTileSet(kWinter);
//	msa = new MapSectorAbstraction(map, 2);
//	msa->ToggleDrawAbstraction(0);
//	msa->ToggleDrawAbstraction(1);
//	msa->ToggleDrawAbstraction(3);
//	msa->ToggleDrawAbstraction(4);
	unitSims.resize(id+1);
	//unitSims[id] = new EpisodicSimulation<xyLoc, tDirection, MapEnvironment>(new MapEnvironment(map,false));
	MapEnvironment* mEnv = new MapEnvironment(map);
	unitSims[id] = new UnitSimulation<xyLoc, tDirection, MapEnvironment>(mEnv);
	unitSims[id]->SetStepType(kMinTime);

	aaAlgorithms = new AnyAngleHOG(mEnv);
	/*
	//analyzer = new SubgoalAnalyzer(mEnv);
	subgoalAA = new SubgoalAA(mEnv, 10);
	thetaStar = new ThetaStar(mEnv);
	blockAStar = new BlockAStar(mEnv);
	fieldAStar = new FieldAStar(mEnv);
	anya = new ANYA(mEnv);
	*/
	
	//ch = new ContractionHierarchy(map, GraphSearchConstants::GetGraph(map));
}

/**
 * Allows you to install any keyboard handlers needed for program interaction.
 */
void InstallHandlers()
{
	InstallKeyboardHandler(MyDisplayHandler, "Toggle Abstraction", "Toggle display of the ith level of the abstraction", kAnyModifier, '0', '9');
	InstallKeyboardHandler(MyDisplayHandler, "Cycle Abs. Display", "Cycle which group abstraction is drawn", kAnyModifier, '\t');
	InstallKeyboardHandler(MyDisplayHandler, "Pause Simulation", "Pause simulation execution.", kNoModifier, 'p');
	InstallKeyboardHandler(MyDisplayHandler, "Clear Units", "Remove all units from simulation.", kAnyModifier, '|');
	InstallKeyboardHandler(MyDisplayHandler, "Step Simulation", "If the simulation is paused, step forward .1 sec.", kNoModifier, 'o');
	InstallKeyboardHandler(MyDisplayHandler, "Step History", "If the simulation is paused, step forward .1 sec in history", kAnyModifier, '}');
	InstallKeyboardHandler(MyDisplayHandler, "Step History", "If the simulation is paused, step back .1 sec in history", kAnyModifier, '{');
	InstallKeyboardHandler(MyDisplayHandler, "Step Abs Type", "Increase abstraction type", kAnyModifier, ']');
	
	InstallKeyboardHandler(MyDisplayHandler, "Screenshot", "Take a screenshot", kAnyModifier, 's');
	InstallKeyboardHandler(MyDisplayHandler, "Graph Display", "Change Graph Display Mode", kNoModifier, 'g');
	InstallKeyboardHandler(MyDisplayHandler, "Last Graph Display", "Revert Graph Display Mode", kShiftDown, 'g');
	InstallKeyboardHandler(MyDisplayHandler, "Area Display", "Change Area Display Mode", kNoModifier, 'v');
	InstallKeyboardHandler(MyDisplayHandler, "Last Area Display", "Revert Area Display Mode", kShiftDown, 'v');
	InstallKeyboardHandler(MyDisplayHandler, "Random Problem", "Generate a Random Problem", kAnyModifier, 'r');
	InstallKeyboardHandler(MyDisplayHandler, "Stored Debug Problem", "Stored Debug Problem", kAnyModifier, 'b');
	InstallKeyboardHandler(MyDisplayHandler, "Solve Again", "Solve the last solved problem again", kAnyModifier, 'e');

	InstallKeyboardHandler(MyPathfindingKeyHandler, "Mapbuilding Unit", "Deploy unit that paths to a target, building a map as it travels", kNoModifier, 'd');
	InstallKeyboardHandler(MyRandomUnitKeyHandler, "Add A* Unit", "Deploys a simple a* unit", kNoModifier, 'a');
	InstallKeyboardHandler(MyRandomUnitKeyHandler, "Add simple Unit", "Deploys a randomly moving unit", kShiftDown, 'a');
	InstallKeyboardHandler(MyRandomUnitKeyHandler, "Add simple Unit", "Deploys a right-hand-rule unit", kControlDown, '1');

	InstallCommandLineHandler(MyCLHandler, "-map", "-map filename", "Selects the default map to be loaded.");
	InstallCommandLineHandler(MyCLHandler, "-convert", "-map file1 file2", "Converts a map and saves as file2, then exits");
	InstallCommandLineHandler(MyCLHandler, "-size", "-batch integer", "If size is set, we create a square maze with the x and y dimensions specified.");

	
	InstallWindowHandler(MyWindowHandler);

	InstallMouseClickHandler(MyClickHandler);
}

void MyWindowHandler(unsigned long windowID, tWindowEventType eType)
{
	if (eType == kWindowDestroyed)
	{
		printf("Window %ld destroyed\n", windowID);
		RemoveFrameHandler(MyFrameHandler, windowID, 0);

		
		delete unitSims[windowID];
		unitSims[windowID] = 0;
		mouseTracking = false;
	}
	else if (eType == kWindowCreated)
	{
		printf("Window %ld created\n", windowID);
		InstallFrameHandler(MyFrameHandler, windowID, 0);
		CreateSimulation(windowID);
		SetNumPorts(windowID, 1);
	}
}

void MyFrameHandler(unsigned long windowID, unsigned int viewport, void *)
{
	if (viewport == 0)
	{
		for(int x = 0; x<gStepsPerFrame; x++)
			unitSims[windowID]->StepTime(1.0/30.0);

	}
	unitSims[windowID]->GetEnvironment()->GetMap()->OpenGLDraw();
	if (unitSims[windowID]->GetNumUnits() == GetNumPorts(windowID)){
		unitSims[windowID]->OpenGLDraw(viewport);
	}
	else{
		unitSims[windowID]->OpenGLDraw();
	}
		
	if (mouseTracking)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		Map *m = unitSims[windowID]->GetEnvironment()->GetMap();
		GLdouble x, y, z, r;
		m->GetOpenGLCoord(px1, py1, x, y, z, r);
		glVertex3f(x, y, z-3*r);
		m->GetOpenGLCoord(px2, py2, x, y, z, r);
		glVertex3f(x, y, z-3*r);
		glEnd();
	}

	static bool done = false;
	
	statValue v;
	
	if(unitSims[windowID]->Done()){
		if(done == false && unitSims[windowID]->GetNumUnits() > 0){
			done = true;
			for(int i = 0; i < unitSims[windowID]->GetNumUnits(); i++){
				int choice = unitSims[windowID]->GetStats()->FindNextStat("nodesExpanded", unitSims[windowID]->GetUnit(i)->GetName(),0);
				if (choice != -1){
					unitSims[windowID]->GetStats()->LookupStat(choice, v);
					std::cout<<"First trial, nodes expanded ";
					std::cout<<unitSims[windowID]->GetUnit(i)->GetName()<<"\t";
					std::cout<<v.lval<<"\n";
					
					std::cout<<"All trials, nodes expanded ";
					std::cout<<unitSims[windowID]->GetUnit(i)->GetName()<<"\t";
					std::cout<<SumStatEntries(unitSims[windowID]->GetStats(), "nodesExpanded", unitSims[windowID]->GetUnit(i)->GetName())<<"\n";
					
					unitSims[windowID]->SetPaused(true);
				}
			}
		}
	}
	

    aaAlgorithms->Visualize(unitSims[windowID]->GetEnvironment());
	//analyzer->OpenGLDraw(unitSims[windowID]->GetEnvironment());
	//subgoalAA->OpenGLDraw(unitSims[windowID]->GetEnvironment());
	//thetaStar->OpenGLDraw(unitSims[windowID]->GetEnvironment());
	//blockAStar->OpenGLDraw(unitSims[windowID]->GetEnvironment());
	//fieldAStar->OpenGLDraw(unitSims[windowID]->GetEnvironment());
	//anya->OpenGLDraw(unitSims[windowID]->GetEnvironment());
}

int MyCLHandler(char *argument[], int maxNumArgs)
{
	if (strcmp(argument[0], "-map") == 0)
	{
		if (maxNumArgs <= 1)
			return 0;
		strncpy(gDefaultMap, argument[1], 1024);

		//doExport();
		return 2;
	}
	else if (strcmp(argument[0], "-convert") == 0)
	  {
		  if (maxNumArgs <= 2)
			  return 0;
		  Map m(argument[1]);
		  m.Save(argument[2]);
		  strncpy(gDefaultMap, argument[1], 1024);
		  return 3;
	  }
	else if (strcmp(argument[0], "-size" ) == 0)
	{
		if (maxNumArgs <= 1)
			return 0;
		mazeSize = atoi(argument[1]);
		assert( mazeSize > 0 );
		return 2;
	}
	return 2; //ignore typos
}

void MyDisplayHandler(unsigned long windowID, tKeyboardModifier mod, char key)
{
	switch (key)
	{

		case 's':
		{
			char filename[20];
			//double epsilon;
			//myFollower->GetEpsilon(epsilon);
			ssCount++;

			sprintf(filename, "%u", ssCount);

			SaveScreenshot(windowID, filename);
			std::cout<<"Screenshot saved! Filename: "<<filename<<std::endl;
		}
			break;

		case 'b':
		{
			//(102, 136) to (240, 263)
			//(236, 255) to (240, 263)
			//(100, 152) to (250, 179)
			//Searching from (141, 211) to (148, 228)
			//(759, 172) to (260, 581)

			//(248, 65) to (134, 52)
			//(188, 112) to (7, 103)
			//(257, 56) to (179, 114)

			//(252, 97) to (220, 112)
			//(192, 142) to (267, 35)

			//(161, 82) to (235, 190)

			// (201, 72) to (15, 109) Example where even path smoothing won't help
			//from = xyLoc(201, 72);
			//to = xyLoc(15, 109);

			//(197, 133) to (221, 151)
			//from = xyLoc(197, 133) ;
			//to = xyLoc(221, 151);

			// Running example
			//from = xyLoc(161, 82);
			//to = xyLoc(235, 190);

			//Searching from (75, 746) to (708, 67)

			// (144, 115) to (177, 73)
			// (107, 118) to (146, 161)
			// (231, 167) to (159, 150)
			// (101, 111) to (260, 110)
			// (96, 13) to (82, 64)
			// (35, 101) to (37, 99)

			// (231, 157) to (78, 108)
			// (149, 98) to (212, 95)
			// (125, 116) to (140, 70)

			//from = xyLoc(125, 116);
			//to = xyLoc(140, 70);

			// TODO: (87, 118) to (235, 163)
			//(124, 68) to (167, 140)

			// Field D* smoothing (84, 69) to (161, 67)
			// (125, 149) to (249, 197)

			//from = xyLoc(87, 118);
			//to = xyLoc(235, 163);

			// 512-40-8: (361, 380) to (505, 54)
			// (455, 143) to (466, 131)
			// (456, 143) to (459, 137)
			// (348, 279) to (231, 379)
			// (286, 348) to (264, 367)

			// rmtst (1,12) to (60,3)

            // arena2 101  78  97  78   // ANYA bug
            //from = xyLoc(101, 78);
            //to = xyLoc(97, 78);

            // random512-40-3 257 301 232 241 // ANYA longer than theta*
            //from = xyLoc(123, 27);
            //to = xyLoc(24, 106);

            from = xyLoc(263, 65);
            to = xyLoc(122, 29);

			// ANYA DEBUG: Failed search from (129, 168) to (126, 24) Possible problem locating the goal because 'point' intervals are not generated

			printf("Searching from (%d, %d) to (%d, %d)\n", from.x, from.y, to.x, to.y);

			aaAlgorithms->FindPaths(from, to);

			/*
			subgoalAA->FindPath(from, to);
			thetaStar->FindPath(from, to);
			blockAStar->FindPath(from,to);
			fieldAStar->FindPath(from,to);
			anya->FindPath(from, to);
			*/
		}
			break;

	/*
		case 'g':
		{
			if (mod != kShiftDown)
				analyzer->ToggleGraphDisplayMode();
			else
				analyzer->RevertGraphDisplayMode();
		}
			break;
		case 'v':
		{
			if (mod != kShiftDown)
				analyzer->ToggleAreaDisplayMode();
			else
				analyzer->RevertAreaDisplayMode();
		}
			break;

	//*/
		case 'r':
		{
			//subgoalAA->GetRandomProblem(from, to);

		    aaAlgorithms->GetRandomProblem(from, to);
            printf("Searching from (%d, %d) to (%d, %d)\n", from.x, from.y, to.x, to.y);
            aaAlgorithms->FindPaths(from, to);

            //subgoalAA->Solve(from, to);
			//thetaStar->Solve(from, to);
			//blockAStar->Solve(from,to);
			//fieldAStar->FindPath(from,to);
			//anya->FindPath(from, to);
		}
			break;
		case 'e':
		{
			printf("Searching from (%d, %d) to (%d, %d)\n", from.x, from.y, to.x, to.y);

            aaAlgorithms->FindPaths(from, to);
			/*
			subgoalAA->FindPath(from, to);
			thetaStar->FindPath(from, to);
			blockAStar->FindPath(from,to);
			fieldAStar->FindPath(from,to);
			anya->FindPath(from, to);
			*/
		}
			break;
		case '[': if (gStepsPerFrame > 2) gStepsPerFrame /= 2; break;
		case ']': gStepsPerFrame *= 2; break;
		case '|': unitSims[windowID]->ClearAllUnits(); break;
		case '\t':
			if (mod != kShiftDown)
				SetActivePort(windowID, (GetActivePort(windowID)+1)%GetNumPorts(windowID));
			else
			{
				SetNumPorts(windowID, 1+(GetNumPorts(windowID)%MAXPORTS));
			}
			break;
		case 'p': unitSims[windowID]->SetPaused(!unitSims[windowID]->GetPaused()); break;
		case 'o':
		{
			if(unitSims[windowID]->GetPaused()){
				unitSims[windowID]->SetPaused(false);
				unitSims[windowID]->StepTime(.02);
				unitSims[windowID]->SetPaused(true);
			}
		}
			break;
		default:
			break;
	}
}

void MyRandomUnitKeyHandler(unsigned long windowID, tKeyboardModifier , char)
{

}

void MyPathfindingKeyHandler(unsigned long windowID, tKeyboardModifier , char)
{

}

bool MyClickHandler(unsigned long windowID, int, int, point3d loc, tButtonType button, tMouseEventType mType)
{
	mouseTracking = false;
	if (button == kRightButton)
	{
		switch (mType)
		{
			case kMouseDown:
			{
				unitSims[windowID]->GetEnvironment()->GetMap()->GetPointFromCoordinate(loc, px1, py1);
				//printf("Mouse down at (%d, %d)\n", px1, py1);
				//unitSims[windowID]->AddUnit(new MovingTargetUnit(px1,py1));
				xyLoc query(px1,py1);
				//analyzer->SetQuery(query);
			}
				break;
			case kMouseDrag:
				mouseTracking = true;
				unitSims[windowID]->GetEnvironment()->GetMap()->GetPointFromCoordinate(loc, px2, py2);
				//printf("Mouse tracking at (%d, %d)\n", px2, py2);
				break;
			case kMouseUp:
			{
				if ((px1 == -1) || (px2 == -1))
					break;
				unitSims[windowID]->GetEnvironment()->GetMap()->GetPointFromCoordinate(loc, px2, py2);

				from = xyLoc(px1,py1);
				to = xyLoc(px2,py2);

				if (from.x != to.x || from.y != to.y)
				{
					printf("Searching from (%d, %d) to (%d, %d)\n", from.x, from.y, to.x, to.y);
		            aaAlgorithms->FindPaths(from, to);
					/*
					subgoalAA->FindPath(from,to);
					thetaStar->FindPath(from,to);
					blockAStar->FindPath(from,to);
					fieldAStar->FindPath(from,to);
					anya->FindPath(from, to);
					*/
				}

				else
				{
					aaAlgorithms->SetDebugLoc(from);
				}

				//analyzer->CompareSearches(start,goal);
				//analyzer->SetQuery(start);
				//analyzer->IsHReachable(start,goal);
				//analyzer->CanLookaheadQuickTravel(start,goal);

//				unitSims[windowID]->ClearAllUnits();

//				myTarget = new MovingTargetUnit(px2,py2);
//				myTarget->SetColor(0,0,1);

//				myFollower = new FollowerUnit(px1,py1,myTarget);
//				myTarget->SetPursuer(myFollower);

//				unitSims[windowID]->AddUnit(myFollower);
//				unitSims[windowID]->AddUnit(myTarget);

//				unitSims[windowID]->AddUnit(new LearningUnit<xyLoc, tDirection, MapEnvironment> (start,goal, new HLRTA::HLRTAStar<xyLoc, tDirection, MapEnvironment>()));
//				unitSims[windowID]->AddUnit(new LearningUnit<xyLoc, tDirection, MapEnvironment> (start,goal, new DALRTA::daLRTAStar<xyLoc, tDirection, MapEnvironment>(10)));
//				unitSims[windowID]->AddUnit(new LearningUnit<xyLoc, tDirection, MapEnvironment> (start,goal, new FLRTA::FLRTAStar<xyLoc, tDirection, MapEnvironment>(10,100)));
//				unitSims[windowID]->AddUnit(new MapLearningUnit(start,goal, new DALRTA::daLRTAStar<xyLoc, tDirection, MapEnvironment>(20)));
//				unitSims[windowID]->AddUnit(new MapLearningUnit(start,goal, new FLRTA::FLRTAStar<xyLoc, tDirection, MapEnvironment>(20,20)));

//				unitSims[windowID]->GetStats()->EnablePrintOutput(true);
//				SetNumPorts(windowID,2);

			}
			break;
		}
		return true;
	}
	return false;
}

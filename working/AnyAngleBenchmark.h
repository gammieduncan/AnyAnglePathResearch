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

#ifndef ANY_ANGLE_BENCHMARK_H
#define ANY_ANGLE_BENCHMARK_H

// ThetaStar
//#define USE_A_STAR			// A* search with Euclidean distance
//#define USE_A_STAR_PS		// A* search with Euclidean distance + post smoothing
//#define USE_A_STAR_OCT		// A* search with Octile distance
//#define USE_A_STAR_OCT_PS	// A* search with Octile distance + post smoothing
//#define USE_THETA_STAR		// Theta* search
//#define USE_THETA_STAR_PS		// Theta* search
//#define USE_LAZY_THETA_STAR	// Lazy Theta* search
//#define USE_LAZY_THETA_STAR_PS		// Lazy Theta* search



// SubgoalAA
//#define USE_SUBGOAL_A_STAR
//#define USE_SUBGOAL_A_STAR_PS
//#define USE_SUBGOAL_THETA_STAR
//#define USE_SUBGOAL_THETA_STAR_PS

// BlockAStar
#define USE_BLOCK_A_STAR
#define USE_BLOCK_A_STAR_PS

// FieldAStar
//#define USE_FIELD_A_STAR
//#define USE_FIELD_A_STAR_PS
#define FIELD_A_STAR_LOOKAHEAD 0

// ANYA
#define USE_ANYA

// Colors
#define Black             0,   0,   0
#define Navy              0,   0, 128
#define DarkGreen         0, 128,   0
#define DarkCyan          0, 128, 128
#define Maroon          128,   0,   0
#define Purple          128,   0, 128
#define Olive           128, 128,   0
#define LightGrey       192, 192, 192
#define DarkGrey        128, 128, 128
#define Blue              0,   0, 255
#define Green             0, 255,   0
#define Cyan              0, 255, 255
#define Red             255,   0,   0
#define Magenta         255,   0, 255
#define Yellow          255, 255,   0
#define White           255, 255, 255
#define Orange          255, 165,   0
#define GreenYellow     173, 255,  47


#define Forest 0,92,49

/*
#define AMETHYST 240,163,255
#define BLUE 0,117,220
#define CARAMEL 153,63,0
#define DAMSON 76,0,92
#define EBONY 25,25,25
#define FOREST 0,92,49
#define GREEN 43,206,72
#define HONEYDEW 255,205,153
#define IRON 128,128,128
#define JADE 148,255,181
#define KHAKI 143,124,0
#define LIME 157,204,0
#define MALLOW 194,9,136
#define NAVY 0,51,128
#define ORPIMENT 255,164,5
#define
{157,204,0}	#9DCC00	 	Lime
{194,0,136}	#C20088	 	Mallow
{0,51,128}	#003380	 	Navy
{255,164,5}	#FFA405	 	Orpiment
{255,168,187}	#FFA8BB	 	Pink
{66,102,0}	#426600	 	Quagmire
{255,0,16}	#FF0010	 	Red
{94,241,242}	#5EF1F2	 	Sky
{0,153,143}	#00998F	 	Turquoise
{224,255,102}	#E0FF66	 	Uranium
{116,10,255}	#740AFF	 	Violet
{153,0,0}	#990000	 	Wine
{255,255,128}	#FFFF80	 	Xanthin
{255,255,0}	#FFFF00	 	Yellow
{255,80,5}	#FF5005	 	Zinnia

#define RED 255,0,0
*/

// Algorithm colors
#define THETA_STAR_COLOR Red
#define SUBGOAL_A_STAR_COLOR DarkGreen
#define SUBGOAL_A_STAR_PS_COLOR Purple
#define SUBGOAL_THETA_STAR_COLOR Navy
#define SUBGOAL_THETA_STAR_PS_COLOR Black


#endif

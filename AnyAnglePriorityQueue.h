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

#ifndef ANY_ANGLE_PRIORITY_QUEUE_H
#define ANY_ANGLE_PRIORITY_QUEUE_H

#include <vector>

template <class Data, class Cmp>
class AnyAnglePriorityQueue
{
public:
	AnyAnglePriorityQueue()	{theHeap.reserve(20000);}

	void Add(Data data)
	{
		theHeap.push_back(data);
		PercolateUp(theHeap.size()-1);
	}
	bool Empty(){return theHeap.empty();}
	Data GetMin(){return theHeap[0];}

	void Clear(){theHeap.clear();}
	void PopMin()
	{
		theHeap[0] = theHeap.back();
		theHeap.pop_back();
		PercolateDown(0);
	}
	void PercolateUp(int index)
	{
		Data elem = theHeap[index];
		int parent = (index-1) >> 1;

		while(index > 0 && compare(elem, theHeap[parent]))
		{
			theHeap[index] = theHeap[parent];
			index = parent;
			parent = (index-1) >> 1;
		}

		theHeap[index] = elem;
	}
	void PercolateDown(int index)
	{
		Data elem = theHeap[index];
		int maxSize = theHeap.size();

		while(true)
		{
			int child1 = (index << 1) + 1;
			if (child1 >= maxSize)
				break;

			int child2 = child1+1;

			// If the first child has smaller key
			if (child2 == maxSize || compare(theHeap[child1],theHeap[child2]))
			{
				if (theHeap[child1].fVal < elem.fVal)
				{
					theHeap[index] = theHeap[child1];
					index = child1;
				}
				else
					break;
			}

			else if (theHeap[child2].fVal < elem.fVal)
			{
				theHeap[index] = theHeap[child2];
				index = child2;
			}
			else
				break;
		}

		theHeap[index] = elem;
	}

private:
	Cmp compare;
	std::vector<Data> theHeap;
};

#endif

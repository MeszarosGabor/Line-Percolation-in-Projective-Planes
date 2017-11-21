// Implementation of the finite projective plane PG(2,p) for p prime
#include <iostream>

struct Triple
{
		Triple (int a=0,int b=0, int c=0):	x(a), y(b), z(c){};
		std::string toString()
		{return "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")";}
		
		int x,y,z;
};

struct ProjectivePlane
{
		ProjectivePlane (int order = 2);
		Triple idToTriple(int id);
		bool adjacent(Triple a, Triple b);
		void displayTriples();
		
		int order_, size_;
		std::vector< std::list<int> > incidences;
};

ProjectivePlane::ProjectivePlane (int order):
	order_(order), size_(order *order + order + 1)
{
	incidences.resize(size_);
	for (int i = 0; i < size_; ++i)
		for (int j = 0; j <= i; ++j)
			if (adjacent(idToTriple(i), idToTriple(j)))
			{
				incidences[i].push_back(j);
				if (i != j){incidences[j].push_back(i);}
			}
}

Triple ProjectivePlane::idToTriple(int id)
{
	if (0<= id && id <= order_*order_ - 1)
		return Triple(id / order_, id % order_, 1);
	else if (order_ * order_ <= id && id <= order_ * order_ + order_ - 1)
		return Triple(id - order_ * order_, 1,0);
	else
		return Triple(1,0,0);
}

bool ProjectivePlane::adjacent(Triple a, Triple b)
{
		return ((a.x * b.x + a.y * b.y + a.z * b.z) % order_ == 0);
}

void ProjectivePlane::displayTriples()
{
	for (int i = 0; i < size_; ++i)
		std::cout << i << ": " << idToTriple(i).toString() << std::endl;
}

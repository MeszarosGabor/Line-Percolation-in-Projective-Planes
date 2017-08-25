//Simulation of the line percolation process on a given projective plane with fixed infection parameter and initially infected set.
#include <fstream>
#include <list>
#include <stdlib.h>    
#include <time.h> 
#include <unordered_set>
#include <vector>

#include "projective_plane.h"

int simulatePercolationWithFixedInitialSet(
						ProjectivePlane & plane,
						int infectionRate, 
						std::unordered_set<int> & initialInfectedSet,
						bool logging = false
						)
{
	std::ofstream logFile;
	if (logging)
	{
		std::string fileName = "line_percolation_order_" + std::to_string(plane.order_) + "_rate_" + std::to_string(infectionRate) + ".txt";
		logFile.open(fileName.c_str());
		logFile << "Percolation on the PG(2," << plane.order_ << ") with infection rate " << infectionRate << std::endl;
	}
	std::vector<int> isPointInfected(plane.size_ , 0);
	std::vector<int> isLineInfected(plane.size_, 0);
	std::vector<int> nrInfectedPointsOnLine(plane.size_, 0);
	std::unordered_set<int> actInfectedPoints;
	std::unordered_set<int> actInfectedLines;
	int oldInfectedPointsCounter = 0;

	actInfectedPoints = initialInfectedSet;
	int round = 0;
	while (!actInfectedPoints.empty())
	{
		++round;
		//ROUND STARTS
		for (std::unordered_set<int>::iterator pointIt = actInfectedPoints.begin(); pointIt != actInfectedPoints.end(); ++pointIt)
		{
			isPointInfected[*pointIt] = 1;
			++oldInfectedPointsCounter;
			for (std::list<int>::iterator linesIt = plane.incidences[*pointIt].begin(); linesIt != plane.incidences[*pointIt].end(); ++linesIt)
			{
				++nrInfectedPointsOnLine[*linesIt];
				if (nrInfectedPointsOnLine[*linesIt] >= infectionRate && isLineInfected[*linesIt] == 0)
					actInfectedLines.insert(*linesIt);
			}
		}
		actInfectedPoints.clear();
		for (std::unordered_set<int>::iterator linesIt = actInfectedLines.begin(); linesIt != actInfectedLines.end(); ++linesIt)
		{
				isLineInfected[*linesIt] = 1;
				for (std::list<int>::iterator pointIt = plane.incidences[*linesIt].begin(); pointIt != plane.incidences[*linesIt].end(); ++pointIt)
					if (isPointInfected[*pointIt] == 0)
						actInfectedPoints.insert(*pointIt);
		}
		actInfectedLines.clear();
		if (logging)
			logFile << "Round " << round << " ends:\n" << "  # of infected points: " << oldInfectedPointsCounter + actInfectedPoints.size() << std::endl << "  # of newly infected points: " << actInfectedPoints.size() << std::endl;
		if (oldInfectedPointsCounter + actInfectedPoints.size() == plane.size_)
		{
			if (logging)
			logFile << "Percolation ended in " << round << "rounds\n";
			logFile.close();
			return round;
		}
	//ROUND ENDS
	}
	if (logging)
	{
		logFile << "Percolation stopped in round " << round <<", the initial set does not percolate\n";
		logFile.close();
	}
	return -1;
}

std::unordered_set<int>  randomSet(
									ProjectivePlane & plane,
									int minSize,
									int maxSize
								)
{
	std::unordered_set<int> points;
	int setSize = rand() % (maxSize - minSize + 1) + minSize;
	while (points.size() < setSize)
				points.insert(rand() % (plane.size_));
	return points;
}

int randomSetSimulation(
							ProjectivePlane & plane,
							int infectionRate,
							int nrOfTests
						)
{
	srand (time(NULL));
	int longestPercolation = -1;
	std::unordered_set<int> slowestInfectedPoints;
	for (int n = 0; n < nrOfTests; ++n)
	{
		std::unordered_set<int> initialInfectedSet = randomSet(
																plane,
																infectionRate * (infectionRate + 1) / 2,
																infectionRate * (infectionRate + 1)
																);
		int actPercolationLength = simulatePercolationWithFixedInitialSet(plane, infectionRate,initialInfectedSet);
		if (actPercolationLength > longestPercolation)
		{
			slowestInfectedPoints = initialInfectedSet;
			longestPercolation = actPercolationLength; 
		}
	}
	std::ofstream myfile;
	std::string fileName = "order_" + std::to_string(plane.order_) + "_rate_" + std::to_string(infectionRate) + "_length_" + std::to_string(longestPercolation) + ".txt";
	myfile.open(fileName.c_str());
	myfile << "#" << " order: " << std::to_string(plane.order_) << std::endl;
	myfile << "#" << " rate: " << std::to_string(infectionRate) << std::endl;
	myfile << "#" << " length of the slowest percolation: " << longestPercolation<< std::endl;
	myfile << "#" << " size of initial infected set: " << slowestInfectedPoints.size()<< std::endl;
	myfile << "#" << " nrOfTests: " << nrOfTests << std::endl;
	for (std::unordered_set<int>::iterator it = slowestInfectedPoints.begin(); it != slowestInfectedPoints.end(); ++it)
		myfile << *it << " " << plane.idToTriple(*it).toString() << std::endl;
	myfile.close();
	return longestPercolation;
}

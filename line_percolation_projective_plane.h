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
						bool logging = false, 
						std::string logFileName = ""
						)
{
	std::ofstream logFile;
	if (logging)
	{
		if (logFileName == "")
			logFileName = "line_percolation_order_" + std::to_string(plane.order_) + "_rate_" + std::to_string(infectionRate) + ".txt";
		logFile.open(logFileName.c_str());
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
			logFile << "Percolation ended in " << round << " rounds\n";
			logFile.close();
			return round;
		}
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
			int nrOfTests, 
			bool logging = true,
			std::string logFileName = ""
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
		int actPercolationLength = simulatePercolationWithFixedInitialSet(plane, infectionRate,initialInfectedSet,false);
		if (actPercolationLength > longestPercolation)
		{
			slowestInfectedPoints = initialInfectedSet;
			longestPercolation = actPercolationLength; 
		}
	}
	if (logging)
	{
		std::ofstream logFile;
		if (logFileName == "")
			logFileName = "line_percolation_random_sim__order_" + std::to_string(plane.order_) + "_rate_" + std::to_string(infectionRate) + ".txt";
		logFile.open(logFileName.c_str());
		logFile << "#" << " order:\n" << std::to_string(plane.order_) << std::endl;
		logFile << "#" << " rate:\n" << std::to_string(infectionRate) << std::endl;
		logFile << "#" << " length of the slowest percolation: " << longestPercolation<< std::endl;
		logFile << "#" << " size of initial infected set: " << slowestInfectedPoints.size()<< std::endl;
		logFile << "#" << " nrOfTests: " << nrOfTests << std::endl;
		for (std::unordered_set<int>::iterator it = slowestInfectedPoints.begin(); it != slowestInfectedPoints.end(); ++it)
			logFile << *it << " " << plane.idToTriple(*it).toString() << std::endl;
		logFile.close();
	}
	return longestPercolation;
}

// this functions reads a previously created set and simulates percolation with that initial set
int simulatePercolationOnGivenInitialSet
					(
					std::string inputFileName,
					bool logging = false,
					std::string logFileName = ""
					)
{
	std::unordered_set<int> infectedPoints;
	std::string line;
	std::ifstream logFile;
	logFile.open(inputFileName.c_str());
	if (logFile.is_open())
	{
		int order, infectionRate, lineCount = 1;
		while(!logFile.eof())
		{
			std::getline(logFile,line);
			if (line.empty() || line[0] == '#'){continue;}
			int id = 0;
			std::string::iterator it = line.begin();
			while (it != line.end() && *it != ' ')
			{
				id *= 10;
				id += (*it - '0');
				++it;
			}
			if (lineCount == 1)
				order = id;
			else if (lineCount == 2)
				infectionRate = id;
			else
				infectedPoints.insert(id);
			++lineCount;
		}
		ProjectivePlane plane(order);
		return simulatePercolationWithFixedInitialSet(plane, infectionRate, infectedPoints, logging, logFileName);
	}
	else 
	{std::cout << "could not open file " << inputFileName << std::endl;}
	logFile.close();
}

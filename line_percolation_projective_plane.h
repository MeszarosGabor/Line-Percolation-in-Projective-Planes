//Simulation of the line percolation process on a given projective plane with fixed infection parameter and initially infected set.
#include <fstream>
#include <list>
#include <unordered_set>
#include <vector>

#include "projective_plane.h"

int fastSimulatePercolation(
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
			return round;
		}
	}
	if (logging)
		logFile << "Percolation stopped in round " << round <<", the initial set does not percolate\n";
	return -1;
}

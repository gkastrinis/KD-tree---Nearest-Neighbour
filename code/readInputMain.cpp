#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sys/time.h>
#include <CGAL/Cartesian_d.h>
#include "KD_tree.hpp"

typedef CGAL::Cartesian_d<double> 	R;
typedef CGAL::Point_d<R> 			PointD;

#define MICROSECS 1000000

using namespace std;


inline void printPoint(PointD p)
{
	for (int i = 0, dimension = p.dimension() ; i < dimension ; i++) cout << p.cartesian(i) << " ";
}

inline void printTimeInterval(string operation, timeval& start, timeval& end)
{
	int sec, msec;
	sec = end.tv_sec - start.tv_sec;
	msec = end.tv_usec - start.tv_usec;
	if ( msec < 0 )
	{
		sec--;
		msec += MICROSECS;
	}
	cout << operation << " time: " << sec << " seconds and " << msec << " microseconds" << endl;
}


int main()
{
	int dimensions;
	cout << "How many dimensions? ";
	cin >> dimensions;
	cout << "\nGive each point in a new line. Separate coordinates with space." << endl;

	vector<PointD> points;
	
	while ( !cin.eof() )
	{
		vector<double> coordinates;
		for (int i = 0 ; i < dimensions ; i++)
		{
			double t;
			cin >> t;
			if ( !cin.good() && !cin.eof() )
			{
				cin.clear();
				break;
			}
			coordinates.push_back(t);
		}
		if ( coordinates.empty() ) break;

		points.push_back( PointD(dimensions, coordinates.begin(), coordinates.end()) );
	}
	
	timeval start, end;

	gettimeofday(&start, NULL);
	KD_tree<PointD> kdTree(dimensions, points);
	gettimeofday(&end, NULL);
	
	printTimeInterval("Tree built", start, end); cout << endl;
	
	if ( !cin.eof() )
	{
		char queries;
		cin >> queries;
		if ( queries != '?' ) return 1;
		
		vector<PointD> queryPoints;
		
		while ( !cin.eof() )
		{
			vector<double> coordinates;
			for (int i = 0 ; i < dimensions ; i++)
			{
				double t;
				cin >> t;
				coordinates.push_back(t);
			}

			queryPoints.push_back( PointD(dimensions, coordinates.begin(), coordinates.end()) );
		}
		
		for (int i = 0, size = queryPoints.size() ; i < size ; i++)
		{
			int sec, msec;
			timeval start, end;

			cout << "\nQuery for point: "; printPoint(queryPoints[i]); cout << endl;
			gettimeofday(&start, NULL);
			PointD neighbour = kdTree.nearestNeighbour(queryPoints[i]);
			gettimeofday(&end, NULL);
			cout << "Neares Neighbour: "; printPoint(neighbour); cout << endl;
			
			printTimeInterval("Query", start, end);
		}
	}
}

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <sys/time.h>
#include <CGAL/Cartesian_d.h>
#include <CGAL/point_generators_d.h>
#include "KD_tree.hpp"

typedef CGAL::Cartesian_d<double> 				R;
typedef CGAL::Point_d<R> 						PointD;
typedef CGAL::Random_points_in_cube_d<PointD>	RandomPoints; 

#define MICROSECS 1000000

using namespace std;
using namespace CGAL;


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


int main(int argc, char* argv[])
{
	if ( argc < 4 )
	{
		cerr << "Wrong number of arguments. Usage: <dimensions> <number of points> <number of queries>" << endl;
		exit(1);
	}
	int dimensions = atoi(argv[1]);
	int number = atoi(argv[2]);
	int queries = atoi(argv[3]);
	
	cout << "Trying for " << dimensions << " dimensions, " << number << " random points and performing " << queries << " queries\n" << endl;

	vector<PointD> points;
	
	RandomPoints g(dimensions, 60);

	for (int i = 0 ; i < number ; i++, g++) points.push_back(*g);
	
	timeval start, end;

	gettimeofday(&start, NULL);
	KD_tree<PointD> kdTree(dimensions, points);
	gettimeofday(&end, NULL);
	
	printTimeInterval("Tree built", start, end); cout << endl;
	
	for (int i = 0 ; i < queries ; i++, g++)
	{
		PointD queryPoint = *g;

		//cout << "\nQuery for point: " << queryPoint << endl;
		gettimeofday(&start, NULL);
		PointD neighbour = kdTree.nearestNeighbour(queryPoint);
		gettimeofday(&end, NULL);
		//cout << "Neares Neighbour: " << neighbour << endl;

		printTimeInterval("Query", start, end);
	}
}

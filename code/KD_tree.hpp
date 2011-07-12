#ifndef KD_TREE_HPP
#define KD_TREE_HPP

#include <iostream>
#include <vector>
#include <algorithm>

template <class PointType> class KD_tree
{
	typedef std::vector<PointType> 				 	  Points;
	typedef typename std::vector<PointType>::iterator PointsIter;
	
	struct Node
	{
		// The node's data
		PointType 	point;
		// The level represents the coordinate that dominates this level of the tree
		int 		level;
		// Left and Right subtrees
		Node* 		less;
		Node* 		greater;
		
		Node(PointType& point_, int level_) : point(point_), level(level_), less(NULL), greater(NULL) {}
	};
	
	// Used in sorting; Sorting is performed based on the dimension'th coordinate
	struct DimensionOrder
	{
		int dimension;
		
		DimensionOrder(int dimension_) : dimension(dimension_) {}
		
		bool operator() (PointType p1, PointType p2) { return p1.cartesian(dimension) < p2.cartesian(dimension);}
	};
	
	struct NeighbourSearch
	{
		int 		dimensions;

		PointType 	currentBest;
		double 	  	currentBestDistance;


		NeighbourSearch(int dimensions_) : dimensions(dimensions_), currentBestDistance(-1.0) {}

		// The code is based on the algorithm described here:
		// http://en.wikipedia.org/wiki/K-d_tree#Nearest_neighbour_search
		PointType search(Node* currentRoot, PointType& queryPoint)
		{
			// Return dummy point
			if ( currentRoot == NULL ) return PointType(dimensions, CGAL::ORIGIN);
			
			int level = currentRoot->level;
			Node* branchToFollow;
			Node* otherBranch;

			// Choose which path to follow, based on current level'th coordinate
			if ( queryPoint.cartesian(level) < currentRoot->point.cartesian(level) )
			{
				branchToFollow = currentRoot->less;
				otherBranch = currentRoot->greater;
			}
			else
			{
				branchToFollow = currentRoot->greater;
				otherBranch = currentRoot->less;
			}

			// Leaf node reached. A leaf node is a node that has no children in the desired direction
			if ( branchToFollow == NULL )
			{
				double currentDistance = simpleDistance(currentRoot->point, queryPoint);

				// First leaf encountered
				if ( currentBestDistance < 0 )
				{
					currentBest = currentRoot->point;
					currentBestDistance = currentDistance;
				}
				else if ( currentDistance < currentBestDistance )
				{
					currentBest = currentRoot->point;
					currentBestDistance = currentDistance;
				}
			}
			else
			{
				search(branchToFollow, queryPoint);

				// Compare currentBest with the currentRoot while unwinding the recursion
				double currentDistance = simpleDistance(currentRoot->point, queryPoint);
				if ( currentDistance < currentBestDistance )
				{
					currentBest = currentRoot->point;
					currentBestDistance = currentDistance;
				}
			}

			// Check if there can be any candidate points on the other side of the splitting plane
			// that can be closer to the queryPoint than the currentBest. In effect check
			// whether a hypersphere with center the queryPoint and radius equal to the distance
			// to the currentBest, crosses over the splitting plane.
			// NOTE: currentBestDistance is a squared distance, and so the distance from the
			// splitting plane is squared as well before the comparison.
			double distanceFromSplit = queryPoint.cartesian(level) - currentRoot->point.cartesian(level);
			if ( (distanceFromSplit * distanceFromSplit)  < currentBestDistance )
				search(otherBranch, queryPoint);
			
			return currentBest;
		}
		
		// Use squared distances in order to avoid computing square roots
		double simpleDistance(PointType a, PointType b)
		{
			double sum = 0.0;
			for (int i = 0 ; i < dimensions ; i++)
			{
				double factor = a.cartesian(i) - b.cartesian(i);
				sum += factor * factor;
			}
			return sum;
		}
	};
	
	//////////////////////////////////////////////////

	// Total dimensions of points
	int 	dimensions;
	// The global root of the tree
	Node* 	root;


	void createTree(Node*& currentRoot, int level, PointsIter begin, PointsIter end)
	{
		// Order points based on the coordinate indicated by level
		DimensionOrder orderer(level);
		sort(begin, end, orderer);
		
		// Find the median in order to perform the split
		int size = end - begin - 1;
		PointsIter median = begin + size / 2;
		currentRoot = new Node(*median, level);
		
		// Create subtrees
		if ( begin != median )
			createTree(currentRoot->less, (level+1) % dimensions, begin, median);
		if ( median + 1 != end )
			createTree(currentRoot->greater, (level+1) % dimensions, median + 1, end);
	}


public:

	KD_tree (int dimensions_, Points& points) : dimensions(dimensions_), root(NULL)
	{
		if ( points.begin() != points.end() )
			createTree(root, 0, points.begin(), points.end());

		//std::cout << "\nPre-Order printing" << std::endl;
		//print(root);
	}
	
	PointType nearestNeighbour(PointType& queryPoint)
	{
		NeighbourSearch neighbourSearch(dimensions);
		return neighbourSearch.search(root, queryPoint);
	}
	
	void print(Node* currentRoot)
	{
		if ( currentRoot == NULL ) return;

		std::cout << "Level: " << currentRoot->level << " | " << currentRoot->point << std::endl;

		if ( currentRoot->less != NULL )
		{
			std::cout << "\nLess (" << currentRoot->point << ")\n---->";
			print(currentRoot->less);
		}
		if ( currentRoot->greater != NULL )
		{
			std::cout << "\nGreater (" << currentRoot->point << ")\n---->";
			print(currentRoot->greater);
		}
	}
};

#endif
#include "Quadtree.h"
#include "Object.h"

Quadtree::Quadtree() :
left( 0 ),
right( 0 ),
top( 0 ),
down( 0 ),
numObjectsToGrow( 4 ),
nodes( 0 ),
isLeaf( true )
{
}

Quadtree::Quadtree( double _left, double _right, double _top, double _down, unsigned int _numObjectsToGrow ) :
left( _left ),
right( _right ),
top( _top ),
down( _down ),
numObjectsToGrow( _numObjectsToGrow ),
nodes( 0 ),
isLeaf( true )
{
}

Quadtree::~Quadtree()
{
	if ( !isLeaf )
		delete [] nodes;
}

void Quadtree::AddObject( Object *object )
{
	if ( isLeaf ) {
		objects.push_back( object );
		bool reachedMaxObjects = ( objects.size() == numObjectsToGrow );
		if ( reachedMaxObjects ) {
			createLeaves();
			moveObjectsToLeaves();
			isLeaf = false;
		}
		return;
	}

	for ( int n = 0; n < NodeCount; ++n ) {
		if ( nodes[n].contains( object ) ) {
			nodes[n].AddObject( object );
			return;
		}
	}

	objects.push_back( object );
}

void Quadtree::Clear()
{
	objects.clear();

	if ( !isLeaf ) {
		for ( int n = 0; n < NodeCount; ++n ) {
			nodes[n].Clear();
		}
	}
}

vector<Object*> Quadtree::GetObjectsAt( double x, double y )
{
	if ( isLeaf ) {
		return objects;
	}

	vector<Object*> returnedObjects;
	vector<Object*> childObjects;

	if ( !objects.empty() )
		returnedObjects.insert( returnedObjects.end(), objects.begin(), objects.end() );

	for ( int n = 0; n < NodeCount; ++n ) {
		if ( nodes[n].contains( x, y ) ) {
			childObjects = nodes[n].GetObjectsAt( x, y );
			returnedObjects.insert( returnedObjects.end(), childObjects.begin(), childObjects.end() );
			break;
		}
	}
	
	return returnedObjects;
}

bool Quadtree::contains( Object *object )
{
	return 	object->left > left &&
		object->right < right &&
		object->top > top &&
		object->down < down;
}

bool Quadtree::contains( double x, double y )
{
	return 	( x >= left && x <= right ) &&
		( y >= top && y <= down );
}

void Quadtree::createLeaves()
{
	nodes = new Quadtree[4];
	nodes[NW] = Quadtree( left, (left+right)/2, top, (top+down)/2, numObjectsToGrow );
	nodes[NE] = Quadtree( (left+right)/2, right, top, (top+down)/2, numObjectsToGrow );
	nodes[SW] = Quadtree( left, (left+right)/2, (top+down)/2, down, numObjectsToGrow );
	nodes[SE] = Quadtree( (left+right)/2, right, (top+down)/2, down, numObjectsToGrow );
}

void Quadtree::moveObjectsToLeaves()
{
	for ( int n = 0; n < NodeCount; ++n ) {
		for ( unsigned int m = 0; m < objects.size(); ++m ) {
			if ( nodes[n].contains( objects[m] ) ) {
				nodes[n].AddObject( objects[m] );
				objects.erase( objects.begin() + m );
				--m;
			}
		}
	}
}

#ifndef _HEAP_H_
#define _HEAP_H_

template <class T>
class Heap {
private:
	int size;
	int * posInHeap;	// index of the tree node that the array element corresponds to
	int * tree;	// index of the array element in this tree node
	T * array;
public:
	Heap( T * A, int n );
	void heapify_up( int idx );
	void heapify_down( int idx );
	void change_val( int arrayElementIndex, T const & newVal );
	int getMinElementArrayIndex();
	void pop();
};

template <class T>
Heap<T>::Heap( T * A, int n ) {
	size = n;
	array = A;
	posInHeap = new int[ n ];
	tree = new int[ n + 1 ];
	for ( int i = 0; i < n; i++ ) {
		posInHeap[ i ] = i + 1;
		tree[ i + 1 ] = i;
	}

	for ( int i = n/2; i > 0; i-- ) {
		heapify_down( i );
	}
}

template <class T>
void Heap<T>::heapify_up( int idx ) {
	int originalElement = tree[ idx ];
	while ( idx > 1 ) {
		if ( array[ originalElement ] < array[ tree[ idx / 2 ] ] ) {
			tree[ idx ] = tree[ idx / 2 ];
			posInHeap[ tree[ idx ] ] = idx;
			idx /= 2;
		}
		else break;
	}
	posInHeap[ originalElement ] = idx;
	tree[ idx ] = originalElement;
}

template <class T>
void Heap<T>::heapify_down( int idx ) {
	int originalElement = tree[ idx ];
	while ( 2 * idx + 1 <= size ) {
		int smallerChild = ( array[ tree[ 2 * idx ] ] < array[ tree[ 2 * idx + 1 ] ] ) ? 2 * idx : 2 * idx + 1;

		if ( array[ tree[ smallerChild ] ] < array[ originalElement ] ) {
			tree[ idx ] = tree[ smallerChild ];
			posInHeap[ tree[ idx ] ] = idx;
		}
		else break;
		idx = smallerChild;
	}
	if ( idx * 2 <= size ) {
		if ( array[ tree[ 2 * idx ] ] < array[ originalElement ] ) {
			tree[ idx ] = tree[ 2 * idx ];
			posInHeap[ tree[ idx ] ] = idx;
			idx = 2 * idx;
		}
	}
	posInHeap[ originalElement ] = idx;
	tree[ idx ] = originalElement;
}

template <class T>
void Heap<T>::change_val( int arrayElementIndex, T const & newVal ) {
	int idx = posInHeap[ arrayElementIndex ];
	bool up = ( newVal < array[ arrayElementIndex ] ) ? true : false;
	array[ arrayElementIndex ] = newVal;
	if ( up ) heapify_up( idx );
	else heapify_down( idx );
}

template <class T>
int Heap<T>::getMinElementArrayIndex() {
	return tree[ 1 ];
}

template <class T>
void Heap<T>::pop() {
	tree[ 1 ] = tree[ size-- ];
	posInHeap[ tree[ 1 ] ] = 1;
	heapify_down( 1 );
}

#endif
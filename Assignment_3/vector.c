#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

static void PrintChar(void* elem, void* fp)
{
	fprintf((FILE*)fp, "%c", *(char*)elem);
	fflush((FILE*)fp);
}

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
	assert(elemSize > 0);
	assert(initialAllocation >= 0);
	if (initialAllocation == 0) {
		v->allocLength = 4;
	}
	else
	{
		v->allocLength = initialAllocation;
	}
	v->logLength = 0;
	v->elemSize = elemSize;
	v->elems = malloc(elemSize * v->allocLength);
	v->freeFn = freeFn;
	assert(v->elems != NULL);
}

void VectorDispose(vector *v)
{
	if (v->freeFn) {
		for (int i = 0; i < v->logLength; i++) {
			void* elem = VectorNth(v, i);
			v->freeFn(elem);
		}
	}
	free(v->elems);
	//printf("\nDispose!\n");
}

int VectorLength(const vector *v)
{ 
	//printf("\nLength!\n");
	int len = v->logLength;
	///printf("\n------VectorLength Function------%d\n", len);
	return len;
}

void *VectorNth(const vector *v, int position)
{ 
	assert(position >= 0 && position < v -> logLength);
	void* nth = (char*)v->elems + position * v->elemSize;
	return nth; 
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
	//printf("----vectorReplace at position: %d, logLength= %d\n", position,v->logLength);
	assert(position >= 0 && position < v->logLength);
	void* dest = (char*)v->elems + position * v->elemSize;
	if (v->freeFn != NULL)
		v->freeFn(dest);
	memcpy(dest, elemAddr, v->elemSize);
	/*
	assert(position >= 0 && position < v->logLength);
	void* positionP = VectorNth(v, position);
	if (v->freeFn != NULL) { v -> freeFn(positionP); }
	memcpy(positionP, elemAddr, v->elemSize);
	*/
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position <= v->logLength);
	if (v->logLength == v->allocLength)
		vectorGrow(v);

	void* oldPos = (char*)v->elems + position * v->elemSize;
	void* newPos = (char*)v->elems + (position + 1) * v->elemSize;
	memmove(newPos, oldPos, (v->logLength - position) * v->elemSize);
	memcpy(oldPos, elemAddr, v->elemSize);
	v->logLength++;
	
	
	/*printf("\n-- - vectorInsert-- - \n" );
	assert(position >= 0 && position <= v->logLength);
	if (v->logLength == v->allocLength) { vectorGrow(v); }

	void* positionP = VectorNth(v, position);
	char ch = *(char*)positionP;
	printf(" Position%c ", ch);

	void* PositionAfter = (char*)v->elems +(position+1)*v->elemSize;
	ch = *(char*)PositionAfter;
	printf(" PositionAfter%c ", ch);

	int count = (v->logLength - position) * v->elemSize;
	printf(" count= %d", count);

	memmove(PositionAfter, elemAddr, (v->logLength - position)*v->elemSize);
	memcpy(positionP, elemAddr, v->elemSize);
	ch = *(char*)positionP;
	printf(" PositionP after insertion%c ", ch);
	v -> logLength++;
	VectorMap(v, PrintChar, stdout);

	//printf("\n");
	*/
}

void VectorAppend(vector *v, const void *elemAddr)
{
	if (v->logLength == v->allocLength) {
		vectorGrow(v);
	}
	void* target = (char*)v->elems + v->elemSize * v->logLength;
	memcpy(target, elemAddr, v->elemSize);
	v->logLength++;
}

void vectorGrow(vector* v) {
	v->allocLength *= 2;
	v->elems = realloc(v->elems, v->elemSize * v->allocLength);
	assert(v->elems != NULL);
}

void VectorDelete(vector *v, int position)
{
	assert(position >= 0 && position < v->logLength);
	void* delP = VectorNth(v, position);
	if (v->freeFn) { v->freeFn(delP); }
	if (position < v->logLength - 1) {
		void *shiftP= VectorNth(v, position+1);
		memmove(delP, shiftP , (v->allocLength - position-1)*v->elemSize);
	}
	v->logLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
	qsort(v->elems, v->logLength, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
	void* elem = (void*)v->elems;
	for (int i = 0; i < v->logLength; i++) {
		mapFn(elem, auxData);
		elem = (char*)elem + v->elemSize;
	}
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
	/*
	void* bsearch(
		const void* key, 
		const void* base, 
		size_t num, 
		size_t size, 
		int (*compar)(const void*, const void*));
	*/
	void* found; //pointer to the found element
	if(isSorted){ //if array is sorted, use bsearch
		found = bsearch(
			key,											//key
			VectorNth(v, startIndex),						//base
			(size_t) (v->logLength-startIndex)*v->elemSize, //num
			(size_t) (v->elemSize),							//size
			searchFn						 //comparison function
			);
		if (found != NULL) {
			//char ch = *(char*)found;
			//printf("*found = %c", ch);
			int position = (int)(((char*)found - (char*)v->elems) / v->elemSize);
			return (position);
		}
	}
	else {
		for (int i = startIndex; i < v->logLength; i++) {
			//printf("\nsearching at index %d\n", i);
			void* position = VectorNth(v, i);
			if (searchFn(key, position) == 0)
				return i;
		}
		/*
		void* lsearch(
			const void* key, 
			void* base, 
			size_t * nelp,
			size_t width,
			int (*compar)(const void*, const void*)
		);
		*/
		/*
		size_t num = ((v->logLength) - startIndex) * v->elemSize;
		printf("\nnum= %d ", (int)num);
		found = lsearch(
			key,											//key
			VectorNth(v, startIndex),						//base
			&num,											//num
			(size_t)(v->elemSize),							//size
			searchFn						 //comparison function
			);
		//v->freeFn(vectorNth(v,);
		*/
	}

	return kNotFound;
} 

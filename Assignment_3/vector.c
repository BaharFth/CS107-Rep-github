#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
		VectorMap(v, v->freeFn, NULL);
	}
	free(v->elems);
	//printf("\nDispose!\n");
}

int VectorLength(const vector *v)
{ 
	//printf("\nLength!\n");
	return 0;
}

void *VectorNth(const vector *v, int position)
{ 
	assert(position >= 0 && position < v -> logLength);
	void* nth = (char*)v->elems + position * v->elemSize;
	return nth; 
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position < v->logLength);
	void* positionP = VectorNth(v, position);
	if (v->freeFn != NULL) { v -> freeFn(positionP); }
	memcpy(positionP, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position < v->logLength);
	if (v->logLength == v->allocLength) { vectorGrow(v); }
	void* positionP = VectorNth(v, position);
	void* PositionAfter = (char*)positionP + v->elemSize;
	memmove(PositionAfter, elemAddr, (v->logLength - position)*v->elemSize);
	memcpy(positionP, elemAddr, v->elemSize);
	v -> logLength++;
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
{}

void VectorSort(vector *v, VectorCompareFunction compare)
{}

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
{ return -1; } 
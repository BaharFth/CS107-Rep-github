#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

//insantiate a new hashset
void HashSetNew(hashset* h, int elemSize, int numBuckets,
    HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
    assert(elemSize > 0);
    assert(numBuckets > 0);
    assert(hashfn != NULL);
    assert(comparefn != NULL);

    h->elemSize = elemSize;
    h->numBuckets = numBuckets;
    h->hashfn = hashfn;
    h->cmpfn = comparefn;
    h->freefn = freefn;
    h->buckets = malloc(sizeof(vector) * numBuckets);
    for (int i = 0; i < numBuckets; i++)
        VectorNew(&h->buckets[i], elemSize, freefn, 4);
}

//dispose of the hashset
void HashSetDispose(hashset* h)
{
    for (int i = 0; i < h->numBuckets; i++)
        VectorDispose(&h->buckets[i]);

    free(h->buckets);
}

//return the number of elements in the hashset
int HashSetCount(const hashset* h)
{
    int c = 0;
    for (int i = 0; i < h->numBuckets; i++)
        c += VectorLength(&h->buckets[i]);

    return c;
}

//apply a function to all elements of the hashset
void HashSetMap(hashset* h, HashSetMapFunction mapfn, void* auxData)
{
    for (int i = 0; i < h->numBuckets; i++)
        for (int j = 0; j < VectorLength(&h->buckets[i]); j++)
            mapfn(VectorNth(&h->buckets[i], j), auxData);
}

//enter an element to a hashset
void HashSetEnter(hashset* h, const void* elemAddr)
{
    assert(elemAddr != NULL);
    //using the hashset's hash function,
    //find the bucket where the element belongs
    int buck = h->hashfn(elemAddr, h->numBuckets);
    assert(buck >= 0 && buck < h->numBuckets);

    //check if the element is already in the bucket
    int fPos = VectorSearch(&h->buckets[buck], elemAddr, h->cmpfn, 0, false);
    //if the element is not in the bucket, 
    //append it to the end of the bucket
    if (fPos == -1) {
        VectorAppend(&h->buckets[buck], elemAddr);
        return;
    }
    //if the element is already in there, replace it
    VectorReplace(&h->buckets[buck], elemAddr, fPos);
}

//look up an element in the hashset
void* HashSetLookup(const hashset* h, const void* elemAddr)
{
    assert(elemAddr != NULL);
    //find the bucket where the element would belong
    int buck = h->hashfn(elemAddr, h->numBuckets);
    assert(buck >= 0 && buck < h->numBuckets);

    //search for the element in the bucket
    //if it does not exist, return Null,
    //otherwise return the address of the element in the bucket
    int fPos = VectorSearch(&h->buckets[buck], elemAddr, h->cmpfn, 0, false);
    if (fPos == -1) return NULL;

    return VectorNth(&h->buckets[buck], fPos);
}
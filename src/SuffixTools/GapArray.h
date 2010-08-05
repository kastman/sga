//-----------------------------------------------
// Copyright 2010 Wellcome Trust Sanger Institute
// Written by Jared Simpson (js18@sanger.ac.uk)
// Released under the GPL
//-----------------------------------------------
//
// GapArray - Data structure and functions used to count 
// the number of times a suffix of a given rank occurs in a data set
//
#ifndef GAPARRAY_H
#define GAPARRAY_H

#include <vector>
#include "BWT.h"
#include "DNAString.h"

// Abstract class for GapArrays
class GapArray
{
    public:
        GapArray() {}
        virtual ~GapArray() {}
        virtual void resize(size_t n) = 0;
        virtual void increment(size_t i) = 0;
        virtual size_t get(size_t i) const = 0;
        virtual size_t size() const = 0;

};

// The simplest representation of a gap array is a vector
// of integers. This has a deterministic size
// and O(1) increment/get but in most cases will
// use (much) more storage than the SparseGapArray since most gap
// counts will be very low (<< INT_MAX). See also SparseGapArray.h
class SimpleGapArray : public GapArray
{
    public:
        SimpleGapArray();
        void resize(size_t n);
        void increment(size_t i);
        size_t get(size_t i) const;
        size_t size() const;

    private:
        typedef uint32_t GAP_TYPE;
        typedef std::vector<GAP_TYPE> GapStorage;
        GapStorage m_data; 
};



//typedef uint32_t GAP_TYPE;
//typedef std::vector<GAP_TYPE> GapArray;

void updateGapArray(const DNAString& w, const BWT* pBWTInternal, GapArray* pGapArray);
void analyzeGapArray(GapArray* pGapArray);

#endif

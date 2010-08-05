///-----------------------------------------------
// Copyright 2009 Wellcome Trust Sanger Institute
// Written by Jared Simpson (js18@sanger.ac.uk)
// Released under the GPL
//-----------------------------------------------
//
// RankProcess - Compute a vector of BWT ranks for
// SequenceWorkItems
//
#include "RankProcess.h"

//
//
//
RankProcess::RankProcess(const BWT* pBWT, bool doReverse, bool removeMode) : m_pBWT(pBWT), 
                                                                             m_doReverse(doReverse), 
                                                                             m_removeMode(removeMode)
{

}

//
RankProcess::~RankProcess()
{

}

// Calculate the vector of ranks for the given sequence
RankVector RankProcess::process(const SequenceWorkItem& workItem)
{
    RankVector out;
    DNAString w = workItem.read.seq;
    if(m_doReverse)
        w.reverse();

    size_t l = w.length();
    int i = l - 1;

    // In add mode, the initial rank is zero and we calculate the rank
    // for the last base of the sequence using just C(a). In remove
    // mode we use the index of the read (in the original read table) as
    // the rank so that ranks calculate correspond to the correct
    // entries in the BWT for the read to remove.
    int64_t rank = 0; // add mode
    if(m_removeMode)
    {
        // Parse the read index from the read id
        std::stringstream ss(workItem.read.id);
        ss >> rank;
    }

    out.push_back(rank);

    // Compute the starting rank for the last symbol of w
    char c = w.get(i);

    // In the case that the starting rank is zero (default
    // in add mode, or if we are removing the first read)
    // there can no occurrence of any characters before this
    // suffix so we just calculate the rank from C(a)
    if(rank == 0)
    {
        rank = m_pBWT->getPC(c);
    }
    else
    {
        rank = m_pBWT->getPC(c) + m_pBWT->getOcc(c, rank - 1);
    }

    out.push_back(rank);
    --i;

    // Iteratively compute the remaining ranks
    while(i >= 0)
    {
        char c = w.get(i);
        rank = m_pBWT->getPC(c) + m_pBWT->getOcc(c, rank - 1);
        //std::cout << "c: " << c << " rank: " << rank << "\n";
        out.push_back(rank);
        --i;
    }
    return out;
}

//
//
//
RankPostProcess::RankPostProcess(GapArray* pGapArray) : m_pGapArray(pGapArray), num_strings(0), num_symbols(0)
{

}

//
void RankPostProcess::process(const SequenceWorkItem& /*item*/, const RankVector& result)
{
    ++num_strings;
    num_symbols += result.size(); // one rank was generated per symbol

    for(RankVector::const_iterator iter = result.begin(); iter != result.end(); ++iter)
    {
        m_pGapArray->increment(*iter);
    }
}

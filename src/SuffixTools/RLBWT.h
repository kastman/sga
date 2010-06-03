//-----------------------------------------------
// Copyright 2009 Wellcome Trust Sanger Institute
// Written by Jared Simpson (js18@sanger.ac.uk)
// Released under the GPL 
//-----------------------------------------------
//
// RLBWT - Run-length encoded Burrows Wheeler transform
//
#ifndef RLBWT_H
#define RLBWT_H

#include "STCommon.h"
#include "Occurrence.h"
#include "SuffixArray.h"
#include "ReadTable.h"
#include "HitData.h"
#include "BWTReader.h"
#include "EncodedString.h"

// Defines
#define RL_COUNT_MASK 0x1F  //00011111
#define RL_SYMBOL_MASK 0xE0 //11100000
#define FULL_COUNT 31
#define RL_SYMBOL_SHIFT 5
#define RLBWT_VALIDATE 1

// A unit of the RLBWT is a pair of a symbol and its count
// The high 3 bits encodes the symbol to store
// The low 5 bits encodes the length of the run
struct RLUnit
{
    RLUnit() : data(0) {}
    RLUnit(char b) : data(1)
    {
        setChar(b);   
    }

    // Returns true if the count cannot be incremented
    inline bool isFull() const
    {
        return (data & RL_COUNT_MASK) == FULL_COUNT;
    }

    inline bool isInitialized() const
    {
        return data > 0;
    }

    // 
    inline void incrementCount()
    {
#ifdef RLBWT_VALIDATE
        assert(!isFull());
#endif
        ++data;
    }

    inline uint8_t getCount() const
    {
#ifdef RLBWT_VALIDATE
        assert((data & RL_COUNT_MASK) != 0);
#endif
        return data & RL_COUNT_MASK;
    }

    // Set the symbol
    inline void setChar(char symbol)
    {
        // Clear the current symbol
        data &= RL_COUNT_MASK;
        
        uint8_t code = BWT_ALPHABET::getRank(symbol);
        code <<= RL_SYMBOL_SHIFT;
        data |= code;
    }

    // Get the symbol
    inline char getChar() const
    {
        uint8_t code = data & RL_SYMBOL_MASK;
        code >>= RL_SYMBOL_SHIFT;
        return BWT_ALPHABET::getChar(code);
    }

    // 
    uint8_t data;
};
typedef std::vector<RLUnit> RLVector;

//
// RLBWT
//
class RLBWT
{
    public:
    
        // Constructors
        RLBWT(const std::string& filename);
        
        //    
        void initializeFMIndex();

        // Append a symbol to the bw string
        void append(char b);

        inline char getChar(size_t idx) const { return m_bwStr.get(idx); }
        inline BaseCount getPC(char b) const { return m_predCount.get(b); }

        // Return the number of times char b appears in bwt[0, idx]
        inline BaseCount getOcc(char b, size_t idx) const { return m_occurrence.get(m_bwStr, b, idx); }

        // Return the number of times each symbol in the alphabet appears in bwt[0, idx]
        inline AlphaCount getFullOcc(size_t idx) const { return m_occurrence.get(m_bwStr, idx); }

        // Return the number of times each symbol in the alphabet appears ins bwt[idx0, idx1]
        inline AlphaCount getOccDiff(size_t idx0, size_t idx1) const { return m_occurrence.getDiff(m_bwStr, idx0, idx1); }

        inline size_t getNumStrings() const { return m_numStrings; } 
        inline size_t getBWLen() const { return m_numSymbols; }
        inline size_t getNumRuns() const { return m_rlString.size(); }

        // Return the first letter of the suffix starting at idx
        inline char getF(size_t idx) const
        {
            size_t ci = 0;
            while(ci < ALPHABET_SIZE && m_predCount.getByIdx(ci) <= idx)
                ci++;
            assert(ci != 0);
            return RANK_ALPHABET[ci - 1];
        }

        // Print the size of the BWT
        void printInfo() const;
        void print(const ReadTable* pRT, const SuffixArray* pSA) const;
        void validate() const;

        // IO
        friend class BWTReader;
        friend class BWTWriter;
        void write(const std::string& filename);

    private:

        static const int DEFAULT_SAMPLE_RATE = 64;

        // Default constructor is not allowed
        RLBWT() {}

        // The O(a,i) array
        Occurrence m_occurrence;

        // The C(a) array
        AlphaCount m_predCount;
        
        // The bw string
        BWTString m_bwStr;

        // The run-length encoded string
        RLVector m_rlString;

        // The number of strings in the collection
        size_t m_numStrings;

        // The total length of the bw string
        size_t m_numSymbols;
};
#endif
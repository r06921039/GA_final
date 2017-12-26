/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#ifndef _CHROMOSOME_H
#define _CHROMOSOME_H

#include "course.h"
#define SLOT 3 // slot per day

class Chromosome
{
    public:
        Chromosome ();
        Chromosome (int n_ell, int);

        ~Chromosome ();

        Chromosome& operator= (const Chromosome & c);

        void init (int n_ell, int);

        int getVal (int index) const;
        void setVal (int index, int val);

        int getFitness (const vector<room> &, const vector<course> &);

        /** real evaluator */
        int evaluate (const vector<room> &, const vector<course> &);

        double oneMax () const;

        bool isEvaluated () const;

        void printf () const;

        int getLength () const;

        double getMaxFitness () const;

    protected:
        int *gene;
        int length;
        int fitness;
        bool evaluated;
        int classroom_num;
};
#endif

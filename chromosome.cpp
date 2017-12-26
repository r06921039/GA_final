/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#include <stdio.h>
#include "myrand.h"
#include "global.h"
#include "chromosome.h"
#include <cstring>

Chromosome::Chromosome ()
{
    length = 0;
    gene = NULL;
    evaluated = false;
}


Chromosome::Chromosome (int n_length, int class_num)
{
    gene = NULL;
    init (n_length, class_num);
}


Chromosome::~Chromosome ()
{
    delete[]gene;
}


void Chromosome::init (int n_length, int class_num)
{
    length = n_length;
    classroom_num = class_num;

    if (gene != NULL)
        delete[]gene;

    gene = new int[length];
    myRand.uniformArray (gene, length, 0, (5*SLOT*classroom_num-1));
    evaluated = false;
}

int Chromosome::getVal (int index) const
{
    if (index < 0 || index > length)
        outputErrMsg ("Index overrange in Chromosome::operator[]");
    return gene[index];
}


void Chromosome::setVal (int index, int val)
{
    if (index < 0 || index > length)
        outputErrMsg ("Index overrange in Chromosome::operator[]");

    gene[index] = val;
    evaluated = false;
}


int Chromosome::getFitness (const vector<room> &r, const vector<course> &c)
{
    if (evaluated)
        return fitness;
    else
        return (fitness = evaluate (r, c));
}

bool Chromosome::isEvaluated () const
{
    return evaluated;
}

int Chromosome::evaluate (const vector<room> &r, const vector<course> &c)
{
    evaluated = true;
    int score = 0;
    string s[5 * SLOT] = {""};
    /*for(int i = 0; i < 5*SLOT; i++){
        ::printf("s= %s\n", s[i].c_str());
    }*/
    for(int i = 0; i < length; i++){
        //int room_index = (gene[i]%(5*SLOT))/SLOT;
        int room_index = (gene[i]%(SLOT*classroom_num))/SLOT;
        int t = SLOT*(gene[i]/(SLOT*classroom_num)) + gene[i]%SLOT ;
        
        // course student < classroom student
        if(c[i].student <= r[room_index].student){ // acceptable
            score += 1; 
        }
        else{ // unacceptable
            score -= 100.0;
        }

        // room is in professor perfer building
        if(c[i].building == r[room_index].building){
            score += 1;
        }

        //professor prefer time
        for (int j = 0; j < int(c[i].time.size()); j++ ){
            if(c[i].time[j] == t){
                score += 1;
                break;
            }
            else if(c[i].time[j] > t){
                break;
            }
        }
        
        // group required don't crash
        
        // professor time don't crash
        
        if (s[t].find(c[i].prof_name) != string::npos){
            //::printf("%s\n", s[t].c_str());
            score -= 100;
        }
        //::printf("%s\n", s[t].c_str());
        s[t] += " " + c[i].prof_name;

    }
    return score;
}


// OneMax
double Chromosome::oneMax () const
{
    int i;
    double result = 0;

    for (i = 0; i < length; i++)
        result += gene[i];

    return result;
}


Chromosome & Chromosome::operator= (const Chromosome & c)
{
    int i;

    if (length != c.length) {
        length = c.length;
        delete[]gene;
        init (length, classroom_num);
    }

    evaluated = c.evaluated;
    fitness = c.fitness;

    for (i = 0; i < length; i++)
        gene[i] = c.gene[i];

    return *this;
}


void Chromosome::printf () const
{
    int i;
    for (i = 0; i < length; i++)
        ::printf ("%i\t", gene[i]);
    ::printf("\n");
}


int Chromosome::getLength () const
{
    return length;
}


double Chromosome::getMaxFitness () const
{
    // For OneMax
    return ((double)300-1e-6);
}

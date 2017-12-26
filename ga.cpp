/***************************************************************************
 *   Copyright (C) 2004 by Tian-Li Yu                                      *
 *   tianliyu@cc.ee.ntu.edu.tw                                             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "global.h"
#include "statistics.h"
#include "myrand.h"
#include "ga.h"
#include <cstring>

GA::GA ()
{
    ell = 0;
    nInitial = 0;
    nCurrent = 0;
    fe = 0;

    nNextGeneration = 0;
    maxGen = -1;
    maxFe = -1;

    population = NULL;
    offspring = NULL;
    selectionIndex = NULL;

    room_vector.clear();
    course_vector.clear();

}


GA::GA (int n_ell, int n_nInitial, int n_selectionPressure, double n_pc, double n_pm, int n_maxGen, int n_maxFe, vector<room> r, vector<course> c)
{
    init (n_ell, n_nInitial, n_selectionPressure, n_pc, n_pm, n_maxGen, n_maxFe, r, c);
}


GA::~GA ()
{

    delete[]population;
    delete[]offspring;
    delete[]selectionIndex;

    room_vector.clear();
    course_vector.clear();
}


void
GA::init (int n_ell, int n_nInitial, int n_selectionPressure, double n_pc,
double n_pm, int n_maxGen, int n_maxFe, vector<room> r, vector<course> c)
{
    int i;

    ell = n_ell;
    nInitial = n_nInitial;
    nCurrent = nInitial;
    selectionPressure = n_selectionPressure;
    pc = n_pc;
    pm = n_pm;
    maxGen = n_maxGen;
    maxFe = n_maxFe;
    room_vector = r;
    course_vector = c;

    population = new Chromosome[nInitial];
    offspring = new Chromosome[nInitial];
    selectionIndex = new int[nInitial];
    int sum = 0;
    for (i = 0; i < nInitial; i++) {
        population[i].init (ell, room_vector.size());
        
        int e = population[i].getFitness(room_vector, course_vector);
        if(e<0){
            sum += 1;
        }
        offspring[i].init (ell, room_vector.size());
    }
    ::printf("illegal chromosome in initial population = %i\n", sum);

    initializePopulation ();
}


void GA::initializePopulation ()
{
    for (int i = 0; i < nInitial; i++)
        population[i].init(ell, room_vector.size());
}

// For now, assuming fixed population size
int GA::getNextPopulation ()
{
    return nCurrent;
}

void GA::selection ()
{
    //rwSelection ();
    tournamentSelection ();
}

// Roulette wheel selection
// This is a O(n^2) implementation
// You can achieve O(nlogn) by using binary search
void GA::rwSelection ()
{
    int i, j;

    // Adjusting population size 
    nNextGeneration = getNextPopulation ();

    if (nNextGeneration != nCurrent) {
        delete[]selectionIndex;
        delete[]offspring;
        selectionIndex = new int[nNextGeneration];
        offspring = new Chromosome[nNextGeneration];

        for (i = 0; i < nNextGeneration; i++)
            offspring[i].init (ell, room_vector.size());
    }

    double totalFitness = 0.0;
    for (i = 0; i < nCurrent; i++) 
	totalFitness += population[i].getFitness(room_vector, course_vector);

    for (i = 0; i < nNextGeneration; i++) {
	double pointer = totalFitness * myRand.uniform();
	int index = -1;
	double partialSum = 0.0;
	for (j = 0; j < nCurrent; j++) {
	    partialSum += population[j].getFitness(room_vector, course_vector);
            if (partialSum >= pointer) {
                index = j;
                break;
            }
	}
	if (index == -1) index = nCurrent - 1;

	selectionIndex[i] = index;
    }

}

// tournamentSelection without replacement
void GA::tournamentSelection ()
{
    int i, j;

    // Adjusting population size 
    nNextGeneration = getNextPopulation ();

    if (nNextGeneration != nCurrent) {
        delete[]selectionIndex;
        delete[]offspring;
        selectionIndex = new int[nNextGeneration];
        offspring = new Chromosome[nNextGeneration];

        for (i = 0; i < nNextGeneration; i++)
            offspring[i].init (ell, room_vector.size());
    }

    int randArray[selectionPressure * nNextGeneration];

    int q = (selectionPressure * nNextGeneration) / nCurrent;
    int r = (selectionPressure * nNextGeneration) % nCurrent;

    for (i = 0; i < q; i++)
        myRand.uniformArray (randArray + (i * nCurrent), nCurrent, 0, nCurrent - 1);

    myRand.uniformArray (randArray + (q * nCurrent), r, 0, nCurrent - 1);

    for (i = 0; i < nNextGeneration; i++) {

        int winner = 0;
        double winnerFitness = -DBL_MAX;

        for (j = 0; j < selectionPressure; j++) {
            int challenger = randArray[selectionPressure * i + j];
            double challengerFitness = population[challenger].getFitness (room_vector, course_vector);

            if (challengerFitness > winnerFitness) {
                winner = challenger;
                winnerFitness = challengerFitness;
            }

        }
        selectionIndex[i] = winner;
    }
}


void GA::crossover ()
{
    int i;

    if ((nNextGeneration & 0x1) == 0) { 
    	// nNextGeneration is even
    	
        for (i = 0; i < nNextGeneration; i += 2)
            pairwiseXO (population[selectionIndex[i]], population[selectionIndex[i + 1]],
                offspring[i], offspring[i + 1]);

    }
    else {
        for (i = 0; i < nNextGeneration - 1; i += 2) {
            pairwiseXO (population[selectionIndex[i]], population[selectionIndex[i + 1]],
                offspring[i], offspring[i + 1]);
        }
        offspring[nNextGeneration - 1] =
            population[selectionIndex[nNextGeneration - 1]];
    }
    //printf("crossover\n");
    //printf("nn: %i\n", nNextGeneration);
    //printf("%i\n",population);
    /*int* A = new int[nNextGeneration];
    for(int i = 0; i < ell; i++){
        if (myRand.uniform () > pc) {
            continue;
        }
        
	    myRand.uniformArray(A, nNextGeneration, 0, nNextGeneration-1);
        for(int t = 0; t < nNextGeneration; t++){
		    offspring[t].setVal(i, population[selectionIndex[A[t]]].getVal(i));
	    }
    }
    delete[] A;*/
}


void GA::pairwiseXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)
{
    if (myRand.uniform () < pc) {
	//twoPointXO (p1, p2, c1, c2);
    uniformXO (p1, p2, c1, c2, 0.5);	
    }
    else {
        c1 = p1;
        c2 = p2;
    }
}

void GA::onePointXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)
{
    int i;
    int crossSite = myRand.uniformInt(1, ell-1);

    for (i = 0; i < crossSite; i++) {
            c1.setVal (i, p1.getVal(i));
            c2.setVal (i, p2.getVal(i));
    }

    for (i = crossSite; i < ell; i++) {
            c1.setVal (i, p2.getVal(i));
            c2.setVal (i, p1.getVal(i));
    }
}

void GA::twoPointXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2)
{
    int i;
    int crossSite_1 = myRand.uniformInt(1, ell-2);
    int crossSite_2 = myRand.uniformInt(crossSite_1, ell-1);
    
    for (i = 0; i < crossSite_1; i++) {
        c1.setVal (i, p1.getVal(i));
        c2.setVal (i, p2.getVal(i));
    }
    
    for (i = crossSite_1; i < crossSite_2; i++) {
        c1.setVal (i, p2.getVal(i));
        c2.setVal (i, p1.getVal(i));
    }
    
    for (i = crossSite_2; i < ell; i++) {
        c1.setVal (i, p1.getVal(i));
        c2.setVal (i, p2.getVal(i));
    }
}

void GA::uniformXO (const Chromosome & p1, const Chromosome & p2, Chromosome & c1, Chromosome & c2, double prob)
{
    int i;

    for (i = 0; i < ell; i++) {
        if (myRand.flip (prob)) {
            c1.setVal (i, p1.getVal(i));
            c2.setVal (i, p2.getVal(i));
        }
        else {
            c1.setVal (i, p2.getVal(i));
            c2.setVal (i, p1.getVal(i));
        }
    }
}

void GA::mutation ()
{
    simpleMutation ();
    //mutationClock ();
}


void GA::simpleMutation ()
{
    int i, j;

    for (i = 0; i < nNextGeneration; i++)
        for (j = 0; j< ell; j++)
            if (myRand.flip(pm)) {
                //int val = offspring[i].getVal(j);
                int val = myRand.uniformInt(0, 5 * SLOT * room_vector.size() - 1);
                offspring[i].setVal(j, val);
            }
}

void GA::mutationClock ()
{
    if (pm <= 1e-6) return; // can't deal with too small pm

    int pointer = (int) (log(1-myRand.uniform()) / log(1-pm) + 1);

    while (pointer < nNextGeneration * ell) {

	int q = pointer / ell;
	int r = pointer % ell;

        int val = myRand.uniformInt(0, 5 * SLOT * room_vector.size() - 1);
        offspring[q].setVal(r, val);

	// Compute next mutation clock
	pointer += (int) (log(1-myRand.uniform()) / log(1-pm) + 1);
    };
}


void GA::showStatistics ()
{

    printf ("Gen:%d  Fitness:(Max/Mean/Min):%f/%f/%f Chromsome Length:%d\n",
        generation, stFitness.getMax (), stFitness.getMean (),
        stFitness.getMin (), population[0].getLength ());
    printf ("best chromosome:");
    population[bestIndex].printf ();
    printf ("\n");
}


void GA::replacePopulation ()
{
    int i;

    if (nNextGeneration != nCurrent) {
        delete[]population;
        population = new Chromosome[nNextGeneration];
    }

    for (i = 0; i < nNextGeneration; i++)
        population[i] = offspring[i];

    nCurrent = nNextGeneration;
}


void GA::oneRun (bool output)
{
    int i;
    selection ();
    crossover ();
    mutation ();
    replacePopulation ();

    double max = -DBL_MAX;
    stFitness.reset ();
    for (i = 0; i < nCurrent; i++) {
        double fitness = population[i].getFitness (room_vector, course_vector);
        if (fitness > max) {
            max = fitness;
            bestIndex = i;
        }
        stFitness.record (fitness);
    }

    if (output)
        showStatistics ();

    generation++;
}


int GA::doIt (bool output)
{
    generation = 0;

    while (!shouldTerminate ()) {
        oneRun (output);
    }
    return generation;
}

int GA::diversity(){
    int r_l = 5 * SLOT * room_vector.size();
    int d[ell][r_l];
    memset(d, 0, sizeof(d[0][0]) * ell * r_l);
    for(int i = 0; i < ell; ++i){
        for(int j = 0; j < r_l; ++j){
            d[i][j] = 0;
        }
    }
    for(int i = 0; i < nCurrent; i++){
        for(int j = 0; j < ell; j++){
            d[j][population[i].getVal(j)] = 1;        
        }
    }
    // sum of square
    int div = 0;
    for(int i = 0; i < ell; i++){
        int e = 0;
        for(int j = 0; j < r_l; j++){
            e += d[i][j];
        }

        div += e*e;
    }
    // multiplication
    /*
    int div = 1;
    for(int i = 0; i < ell; i++){
        int e = 0
        for(int j = 0; j < r_l; j++){
            e += d[i][j];
        }
        div *= e;
    }
    */
    return div;
}

bool GA::shouldTerminate ()
{
    bool termination = false;

    // Reach maximal # of function evaluations
    if (maxFe != -1) {
        if (fe > maxFe)
            termination = true;
    }

    // Reach maximal # of generations
    if (maxGen != -1) {
        if (generation > maxGen)
            termination = true;
    }

    // Found a satisfactory solution
    if (stFitness.getMean() >= population[0].getMaxFitness()){
        termination = true;
        ::printf("found a satisfactory solution\n");
    }

    // The population loses diversity
    if (diversity() < 0){
        termination = true;
        ::printf("The population loses diversity\n");
    }
    if (stFitness.getMax()-1e-6 < stFitness.getMean())
	    termination = true;

    return termination;

}

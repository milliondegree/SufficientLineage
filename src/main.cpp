/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: shaobo
 *
 * Created on October 12, 2018, 3:32 AM
 */

#include <cstdlib>
#include <iostream>
#include <cctype>
#include <ctime>
#include <unordered_map>
#include <assert.h>
#include "Load.h"
#include "DNF.h"
#include "Suff.h"
#include "Influ.h"
#include "Change.h"

using namespace std;

// #define ProvPath "./data/"
///#define DataPath "/home/shaobo/Programs/CLionProjects/SuffLineage/data/trust/500_nodes_all.csv"

/*
 * main method to compute the sufficient lineage given a k-mDNF formula and 
 * an approximation error epsilon greater than 0
 */
int main(int argc, char** argv) {

    unordered_map<string, string> args;
    vector<string> argvs;
    for (int i=0; i<argc; i++) {
        argvs.push_back(string(argv[i]));
    }
    for (int i=1; i<argc; i+=2) {
        assert(i+1<argc);
	if (argvs[i]=="-p") {
	    args["p"] = argvs[i+1];
	}
	else if (argvs[i]=="-d") {
	    args["d"] = argvs[i+1];
	}
	else if (argvs[i]=="-e") {
	    args["e"] = stod(argvs[i+1]);
	}
    }

    string DataPath = args["d"];
    string ProvPath = args["p"];

    Load load(DataPath, ProvPath);

    cout << "------------------------------" << endl;
    
    string prov = load.getProv();
    map<string, double> probs = load.getProbs();

    DNF dnf(load.getProv(), load.getProbs());

    cout<<"DNF number of monomials: "<<dnf.getLambda().size()<<endl;
    // dnf.ShowStructure();

    cout << "------------------------------" << endl;

    // double errRate = 0.01; // approximation error rate
    double errRate = atof(args["e"].c_str());
    Suff suff(dnf.getLambda(), errRate);

    cout << "The original DNF is: " << endl;
    suff.printDNF(suff.getOrigDNF());
    // print results
    cout << "Original DNF size: " << suff.getOrigDNF().size() << endl;
    cout << "Original Probability = " << suff.getOrigProb() << endl;
    cout << endl;
    cout << "The sufficient DNF is: " << endl;
    suff.printDNF(suff.getSuffDNF());
    cout << "Sufficient DNF size: " << suff.getSuffDNF().size() << endl;
    cout << "SuffProv Probability = " << suff.getSuffProb() << endl;

    cout << "------------------------------" << endl;
    exit(0);

    Influ influ(suff.getOrigDNF(), suff.getOrigProb());
    cout << "Original lambda influence: " << endl;
    influ.printInflu(influ.getInfluence(0));
    cout << "Distinct literals in original lambda: " << influ.getInfluence(0).size() << endl;
    cout << endl;

    Influ influSuff(suff.getSuffDNF(), suff.getSuffProb());
    cout << "Sufficient lineage influence: " << endl;
    influSuff.printInflu(influSuff.getInfluence(0));
    cout << "Total number of literals in sufficient lineage: " << influSuff.getInfluence(0).size() << endl;
    cout << endl;

    cout << "------------------------------" << endl;

    Change change(influ.getDNF(), influ.getDNFProb(), influ.getInfluence(1).front().first, influ.getDNFProb()-0.5, true);
    cout << "Changing order: " << endl;
    change.printChangeOrder(change.getChangeOrder());

    return 0;
}


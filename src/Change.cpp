//
// Created by shaobo on 9/21/19.
//

#include "Change.h"
#include "Influ.h"
#include "Suff.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <set>

using namespace std;

Change::Change() {

}

Change::Change(const Change & orig) {

}

Change::~Change() {

}

Change::Change(vector< map<string, double> > lambda, double lambdaProb, string topLiteral, double target, bool suffOn) {
    clock_t t = clock();
    Change::setChangeOrder(lambda, lambdaProb, topLiteral, target, suffOn);
    t = clock() - t;
    cout << "Changing order running time: " << ((float) t)/CLOCKS_PER_SEC << " seconds" << endl;
}

void Change::setChangeOrder(vector< map<string, double> > lambda, double lambdaProb, string topLiteral, double target, bool suffOn) {
    cout << "Target prob = " << target << endl;
    cout << "Current prob = "<< lambdaProb << endl;
    if(target == lambdaProb) {
        cout << "No change needed!" << endl;
    } else if(lambdaProb < target) {
        // increase the prob of the top influ literal to 1, then do MC
        vector< map<string, double> > DNFTemp(lambda);
        for(vector< map<string, double> >::iterator mono = DNFTemp.begin(); mono != DNFTemp.end(); mono++) {
            if(mono->find(topLiteral) != mono->end() ) {
                mono->erase(topLiteral);
            }
        }
        double probTemp = Suff::monteCarloSim(DNFTemp);
        //double probTemp = Suff::shannonExpan(DNFTemp);
        cout<<"Changed literal: "<<topLiteral<<" New prob = "<<probTemp<<endl;
        if(probTemp < target) {
            // keep running
            this->changeOrder.emplace_back(topLiteral + " current prob =", probTemp);
            // find suffDNF for DNFTemp and compute its most influential tuple
            Suff suff;
            Influ influ;
            if(suffOn) {
                suff.setOrigDNF(DNFTemp);
                suff.setOrigProb(DNFTemp);
                suff.setSuffDNF(0.01);
                suff.setSuffProb(suff.getSuffDNF());
                influ.setDNF(suff.getSuffDNF());
                influ.setDNFProb(suff.getSuffProb());
                influ.setInfluence(suff.getSuffDNF());
            } else {
                influ.setDNF(DNFTemp);
                influ.setDNFProb(probTemp);
                influ.setInfluence(DNFTemp);
            }
            cout << "Top influ literal: ";
            influ.printInflu(influ.getInfluence(1));
            cout << endl;
            Change::setChangeOrder(DNFTemp, probTemp, influ.getInfluence(1).front().first, target, suffOn);
        } else {
            double delta_p = (target - lambdaProb) / Influ::computeInflu(lambda, topLiteral);
            this->changeOrder.emplace_back(topLiteral + " delta_p =", delta_p);
        }

    } else {
        // decrease the prob of the top influ literal to 0, then do MC
        vector< map<string, double> > DNFTemp;
        for(vector< map<string, double> >::iterator mono = lambda.begin(); mono != lambda.end(); mono++) {
            if(mono->find(topLiteral) == mono->end() ) {
                DNFTemp.push_back((*mono));
            }
        }
        double probTemp = Suff::monteCarloSim(DNFTemp);
        //double probTemp = Suff::shannonExpan(DNFTemp);
        cout<<"Changed literal: "<<topLiteral<<" New prob = "<<probTemp<<endl;
        if(probTemp > target) {
            this->changeOrder.emplace_back(topLiteral + " current prob =", probTemp);
            // find suffDNF for DNFTemp and compute its most influential tuple
            Suff suff;
            Influ influ;
            if(suffOn) {
                suff.setOrigDNF(DNFTemp);
                suff.setOrigProb(DNFTemp);
                suff.setSuffDNF(0.01);
                suff.setSuffProb(suff.getSuffDNF());
                influ.setDNF(suff.getSuffDNF());
                influ.setDNFProb(suff.getSuffProb());
                influ.setInfluence(suff.getSuffDNF());
            } else {
                influ.setDNF(DNFTemp);
                influ.setDNFProb(probTemp);
                influ.setInfluence(DNFTemp);
            }
            cout << "Top influ literal: ";
            influ.printInflu(influ.getInfluence(1));
            cout << endl;
            Change::setChangeOrder(DNFTemp, probTemp, influ.getInfluence(1).front().first, target, suffOn);
        } else {
            double delta_p = (target - lambdaProb) / Influ::computeInflu(lambda, topLiteral);
            this->changeOrder.emplace_back(topLiteral + " delta_p =", delta_p);

        }
    }
}

vector< pair<string, double> > Change::getChangeOrder() {
    return this->changeOrder;
}

void Change::printChangeOrder(vector< pair<string, double> > order) {
    for(vector< pair<string, double> >::iterator it = order.begin(); it != order.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }
}

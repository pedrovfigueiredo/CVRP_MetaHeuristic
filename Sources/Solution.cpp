//
//  Solution.cpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 14/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#include "../Headers/Solution.hpp"

Solution::Solution(const std::vector< std::vector<float> >& adjMatrix, const std::vector<int>& demands, const unsigned nVehicles, const unsigned capacity, const float alpha):
modifiedRoutes_(true), currCost_(0), adjMatrix_(adjMatrix), demands_(demands),nVehicles_(nVehicles), capacity_(capacity){
    this->generateFirstSolution(alpha);
}

Solution::Solution(const Solution* s1):
demands_(s1->demands_),
capacity_(s1->capacity_),
nVehicles_(s1->nVehicles_),
adjMatrix_(s1->adjMatrix_),
vehicles_{s1->vehicles_},
modifiedRoutes_{s1->modifiedRoutes_},
currCost_(s1->currCost_)
{}


Solution::~Solution(){}

void Solution::vizinhoQualquer(unsigned n){
    srand(time(0));
    if (n == 1){
        unsigned r1 = rand()%this->nVehicles_;
        
        while (this->vehicles_[r1].route.size() == 1)
            r1 = rand()%this->nVehicles_;
        
        
        unsigned r2 = rand()%this->vehicles_[r1].route.size(), r3 = rand()%this->vehicles_[r1].route.size();
        
        while (r3 == r2)
            r3 = rand()%this->vehicles_[r1].route.size();
        
        std::iter_swap(this->vehicles_[r1].route.begin() + r2, this->vehicles_[r1].route.begin() + r3);
        return;
    }
    
    while (n-- > 1) {
        while (true) {
            // 0 for swap and 1 for DD
            unsigned swapOrDD = rand()%2;
            
            unsigned r1 = rand()%this->nVehicles_,r2 = rand()%this->nVehicles_;
            
            while (r2 == r1)
                r2 = rand()%this->nVehicles_;
            
            if (swapOrDD == 1){ // DD
                if (this->randomDragAndDrop(this->vehicles_[r1], this->vehicles_[r2]))
                    break;
            }else{ // Swap
                if (this->swapRandomBetweenRoutes(this->vehicles_[r1], this->vehicles_[r2]))
                    break;
            }
        }
    }
}

inline bool Solution::randomDragAndDrop(Vehicle& v1, Vehicle& v2){
    unsigned r1 = rand()%v1.route.size(), r2 = rand()%v2.route.size();
    
    const int currLoad2 = v2.getCurrLoad(this->demands_);
    
    // Vehicle 2 can load clients demands
    if (currLoad2 + this->demands_[v1.route[r1]] > this->capacity_)
        return false;
    
    v2.route.insert(v2.route.begin() + r2, v1.route[r1]);
    v1.route.erase(v1.route.begin() + r1);
    
    return true;
}

inline bool Solution::swapRandomBetweenRoutes(Vehicle& v1, Vehicle& v2){
    unsigned r1 = rand()%v1.route.size(), r2 = rand()%v2.route.size();
    
    const int currLoad1 = v1.getCurrLoad(this->demands_);
    const int currLoad2 = v2.getCurrLoad(this->demands_);
    
    // Vehicles can load clients demands
    if (currLoad1 - this->demands_[v1.route[r1]] + this->demands_[v2.route[r2]] > this->capacity_ ||
        currLoad2 - this->demands_[v2.route[r2]] + this->demands_[v1.route[r1]] > this->capacity_)
        return false;
    
    std::iter_swap(v1.route.begin() + r1, v2.route.begin() + r2);
    
    return true;
}

void Solution::descida1opt(){
    for (int i = 0; i < this->nVehicles_;) {
        if(!swapInRoute(this->vehicles_[i]))
            i++;
    }
}

void Solution::descida2opt(){
    for (int i = 0; i < this->nVehicles_ - 1; i++)
        for (int j = i + 1; j < this->nVehicles_; j++)
            if(swapBetweenRoutes(this->vehicles_[i], this->vehicles_[j])){
                i = 0;
                j = 1;
            }
}

bool Solution::twoOptSwap(Vehicle& vehicle){
    int rsize = (int) vehicle.route.size();
    if (rsize < 2)
        return false;
    
    const float initialCost = vehicle.getCost(adjMatrix_);
    
startAgain:
    float bestCost = vehicle.getCost(adjMatrix_);
    for (int i = 0; i < rsize - 1; i++) {
        for (int j = i + 1; j < rsize; j++) {
            
            std::vector<int> newRoute;
            newRoute.reserve(vehicle.route.size());
            this->twoOpt(newRoute, vehicle.route, i, j);
            
            // Calculating new cost
            float newCost = 0;
            int i = 0;
            for (auto city : newRoute){
                newCost += this->adjMatrix_[i][city];
                i = city;
            }
            newCost += this->adjMatrix_[i][0];
            
            if (newCost < bestCost){
                vehicle.route = newRoute;
                goto startAgain;
            }
            
        }
    }
    if (bestCost == initialCost)
        return false;
    
    return true;
}

void Solution::twoOpt(std::vector<int>& newRoute, const std::vector<int>& route, const int i, const int j){
    for(int k = 0; k < i; k++)
        newRoute.push_back(route[k]);
    
    for (int k = j; k >= i ; k--)
        newRoute.push_back(route[k]);
    
    for (int k = j + 1; k < route.size(); k++) {
        newRoute.push_back(route[k]);
    }
}

bool Solution::dragAndDrop(Vehicle& v1, Vehicle& v2){
    
    int bestI = -1, bestJ = -1;
    float bestBothCost = std::numeric_limits<float>::max();
    
    const int currLoad2 = v2.getCurrLoad(this->demands_);
    const int originalSizev2Route = v2.route.size();
    
    const float initialBothCost = v1.getCost(adjMatrix_) + v2.getCost(adjMatrix_);
    for (int i = 0; i < v1.route.size(); i++) {
        for (int j = 0; j < v2.route.size(); j++) {
            
            // Vehicle 2 can load clients demands
            if (currLoad2 + this->demands_[v1.route[i]] > this->capacity_)
                continue;
            
            v2.route.insert(v2.route.begin() + j, v1.route[i]);
            v1.route.erase(v1.route.begin() + i);
            
            float newBothCost = v1.getCost(adjMatrix_) + v2.getCost(adjMatrix_);
            if (newBothCost < initialBothCost){
                if (bestBothCost > newBothCost) {
                    bestBothCost = newBothCost;
                    bestI = i;
                    bestJ = j;
                }
            }else if(j == originalSizev2Route - 1){
                v2.route.push_back(v2.route[j]);
                v2.route.erase(v2.route.begin() + j);
                newBothCost = v1.getCost(adjMatrix_) + v2.getCost(adjMatrix_);
                if (newBothCost < initialBothCost){
                    if (bestBothCost > newBothCost) {
                        bestBothCost = newBothCost;
                        bestI = i;
                        bestJ = j;
                    }
                }
                v1.route.insert(v1.route.begin() + i, *(v2.route.end()));
                v2.route.erase(v2.route.end() - 1);
                break;
            }
            
            // Revert to initial
            v1.route.insert(v1.route.begin() + i, v2.route[j]);
            v2.route.erase(v2.route.begin() + j);
            
        }
    }
    
    if (bestBothCost != std::numeric_limits<float>::max()){
        // Insert
        v2.route.insert(v2.route.begin() + bestJ, v1.route[bestI]);
        v1.route.erase(v1.route.begin() + bestI);
        this->modifiedRoutes_ = true;
        return true;
    }
    
    return false;
    
}

// VERIFICAR DRAG AND DROP INTEGRADO
bool Solution::swapBetweenRoutes(Vehicle& v1, Vehicle& v2){
    
    int bestI = -1, bestJ = -1;
    float bestBothCost = std::numeric_limits<float>::max();
    
    const int currLoad1 = v1.getCurrLoad(this->demands_);
    const int currLoad2 = v2.getCurrLoad(this->demands_);
    
    const float initialBothCost = v1.getCost(adjMatrix_) + v2.getCost(adjMatrix_);
    for (int i = 0; i < v1.route.size(); i++) {
        for (int j = 0; j < v2.route.size(); j++) {
            
            // Vehicles can load clients demands
            if (currLoad1 - this->demands_[v1.route[i]] + this->demands_[v2.route[j]] > this->capacity_ ||
                currLoad2 - this->demands_[v2.route[j]] + this->demands_[v1.route[i]] > this->capacity_)
                continue;
            
            std::iter_swap(v1.route.begin() + i, v2.route.begin() + j);
            
            float newBothCost = v1.getCost(adjMatrix_) + v2.getCost(adjMatrix_);
            
            if (newBothCost < initialBothCost)
                if (bestBothCost > newBothCost) {
                    bestBothCost = newBothCost;
                    bestI = i;
                    bestJ = j;
                }
            
            // Revert to initial
            std::iter_swap(v1.route.begin() + i, v2.route.begin() + j);
            
        }
    }
    
    if (bestBothCost != std::numeric_limits<float>::max()){
        // Swapping
        std::iter_swap(v1.route.begin() + bestI, v2.route.begin() + bestJ);
        this->modifiedRoutes_ = true;
        return true;
    }
    
    return false;
}

bool Solution::swapInRoute(Vehicle& vehicle){
    int rsize = (int) vehicle.route.size();
    if (rsize < 2)
        return false;
    
    int bestI = -1, bestJ = -1;
    float bestCost = std::numeric_limits<float>::max();
    
    const float initialCost = vehicle.getCost(adjMatrix_);
    for (int i = 0; i < rsize - 1; i++) {
        for (int j = i + 1; j < rsize; j++) {
            
            std::iter_swap(vehicle.route.begin() + i, vehicle.route.begin() + j);
            
            float newCost = vehicle.getCost(adjMatrix_);
            if (newCost < initialCost)
                if (bestCost > newCost) {
                    bestCost = newCost;
                    bestI = i;
                    bestJ = j;
                }
            
            
            // Reverts to initial
            std::iter_swap(vehicle.route.begin() + i, vehicle.route.begin() + j);
            
        }
    }
    
    if (bestCost != std::numeric_limits<float>::max()){
        std::iter_swap(vehicle.route.begin() + bestI, vehicle.route.begin() + bestJ);
        this->modifiedRoutes_ = true;
        return true;
    }
    
    return false;
}

// Heurística de Construção
void Solution::generateFirstSolution(const float alpha){
    srand(time(0));
    std::vector< std::pair<int, int> > unvisitedClients;
    
    for (int i = 1; i < this->adjMatrix_[0].size(); i++)
        unvisitedClients.push_back({i,this->demands_[i]});
    
    std::sort(unvisitedClients.begin(), unvisitedClients.end(), [](const auto a1, const auto a2){return a1.second > a2.second;});
    
    // Visit to first clients of each vehicle
    this->vehicles_.resize(this->nVehicles_);
    for(auto& vehicle : this->vehicles_){
        vehicle.route.push_back(unvisitedClients.front().first);
        unvisitedClients.erase(unvisitedClients.begin());
    }
    
    for (int i = 0; i < unvisitedClients.size(); i++){
        std::vector< std::pair< std::pair<int, int> , float> > visitedClients;
        
        // Adds only clients which have been visited and do not surpass vehicle capacity
        for(int j = 0; j < this->vehicles_.size(); j++)
            for (int k = 0; k < this->vehicles_[j].route.size(); k++)
                if (vehicles_[j].getCurrLoad(this->demands_) + unvisitedClients[i].second <= this->capacity_)
                    visitedClients.push_back({{j,k},adjMatrix_[i][this->vehicles_[j].route[k]]});
        
        // Sorts according to distance
        std::sort(visitedClients.begin(), visitedClients.end(), [](const auto a1, const auto a2){return a1.second < a2.second;});
        
        unsigned int nPossibleCandidates = (visitedClients.size() * alpha) < 1 ? 1 : visitedClients.size() * alpha;
        unsigned int r1 = rand()%nPossibleCandidates;
        
        unsigned int vehicleIndex = visitedClients[r1].first.first, clientIndex = visitedClients[r1].first.second;
        this->vehicles_[vehicleIndex].route.insert(this->vehicles_[vehicleIndex].route.begin() + clientIndex + 1, unvisitedClients[i].first);
    }
    
    this->modifiedRoutes_ = true;
}

void Solution::operator=(const Solution& s){
    this->modifiedRoutes_ = s.modifiedRoutes_;
    this->vehicles_ = s.vehicles_;
    this->currCost_ = s.currCost_;
}

void Solution::printRoutes(){
    int i = 0;
    for (auto vehicle : this->vehicles_){
        std::cout << "Rota " << ++i << ": ";
        for (auto client: vehicle.route)
            std:: cout << client << " ";
        std::cout << "Carga: " << vehicle.getCurrLoad(this->demands_) << " Custo: " << vehicle.getCost(adjMatrix_) << std::endl;
    }
}

int Solution::getCost(){
    
     // Avoids unecessary calculation
     if (!modifiedRoutes_)
     return this->currCost_;
    
    float solution = 0;
    
    for (auto vehicle : this->vehicles_)
        solution += vehicle.getCost(this->adjMatrix_);
    
    this->modifiedRoutes_ = false;
    this->currCost_ = solution;
    return (int) solution;
}


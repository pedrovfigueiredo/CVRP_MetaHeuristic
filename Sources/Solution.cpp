//
//  Solution.cpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 14/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#include "../Headers/Solution.hpp"

Solution::Solution(const std::vector< std::vector<float> >& adjMatrix, const std::vector<int>& demands, const unsigned nVehicles, const unsigned capacity, RelaxationLevel level):
modifiedRoutes_(true), currCost_(0), adjMatrix_(adjMatrix), demands_(demands),nVehicles_(nVehicles), capacity_(capacity){
    this->generateFirstSolution(level);
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
void Solution::generateFirstSolution(RelaxationLevel level){
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
    
    switch (level) {
        case Total:
            srand( (unsigned) time(0));
            for (int i = 0; i < unvisitedClients.size(); i++){
                for(int j = 0; j < this->adjMatrix_.size(); j++) {
                    int vehicle = (rand()%this->nVehicles_);
                    int client = (rand()%(this->vehicles_[vehicle].route.size()));
                    if (this->vehicles_[vehicle].getCurrLoad(this->demands_) + this->demands_[unvisitedClients[i].first] <= this->capacity_) {
                        this->vehicles_[vehicle].route.insert(this->vehicles_[vehicle].route.begin() + client + 1, unvisitedClients[i].first);
                        break;
                    }
                }
            }
            break;
            
        case None:
            
            for (int i = 0; i < unvisitedClients.size(); i++){
                int bestVehicleIndex = -1, bestClientIndex = -1;
                float minDistance = std::numeric_limits<float>::max();
                
                // Descobre qual o cliente já visitado mais próximo que cabe no veículo
                for(int j = 0; j < this->vehicles_.size(); j++){
                    for (int k = 0; k < this->vehicles_[j].route.size(); k++){
                        if (adjMatrix_[i][this->vehicles_[j].route[k]] < minDistance && vehicles_[j].getCurrLoad(this->demands_) + this->demands_[unvisitedClients[i].first] <= this->capacity_) {
                            minDistance = adjMatrix_[i][this->vehicles_[j].route[k]];
                            bestVehicleIndex = j;
                            bestClientIndex = k;
                        }
                    }
                }
                
                // Atualiza rota escolhida
                this->vehicles_[bestVehicleIndex].route.insert(this->vehicles_[bestVehicleIndex].route.begin() + bestClientIndex + 1, unvisitedClients[i].first);
            }
            break;
        default:
            std::cerr << "Opção de relaxação inválida." << std::endl;
            break;
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

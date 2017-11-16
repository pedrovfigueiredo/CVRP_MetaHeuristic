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
    
    for (int i = 0; i < 50000; i++) {
        swapBetweenRoutes();
        swapOneInRoute(vehicles_[rand()%nVehicles]);
        int v = rand()%nVehicles;
        shiftNInRoute(vehicles_[v], rand()%(vehicles_[v].route.size()));
    }
}

Solution::~Solution(){}

bool Solution::swapBetweenRoutes(){
    int v1 = rand()%this->vehicles_.size();
    int v2 = rand()%this->vehicles_.size();
    while (v1 == v2)
        v2 = rand()%this->vehicles_.size();
    
    int client1 = rand()%this->vehicles_[v1].route.size();
    int client2 = rand()%this->vehicles_[v2].route.size();
    
    int currLoad1 = this->vehicles_[v1].currLoad;
    int currLoad2 = this->vehicles_[v2].currLoad;
    
    // Vehicles can load clients demands
    if (currLoad1 - this->demands_[this->vehicles_[v1].route[client1]] + this->demands_[this->vehicles_[v2].route[client2]] > this->capacity_ ||
        currLoad2 - this->demands_[this->vehicles_[v2].route[client2]] + this->demands_[this->vehicles_[v1].route[client1]] > this->capacity_)
        return false;
    
    int costBoth = this->vehicles_[v1].getCost(adjMatrix_) + this->vehicles_[v2].getCost(adjMatrix_);
    
    std::iter_swap(this->vehicles_[v1].route.begin() + client1, this->vehicles_[v2].route.begin() + client2);
    
    if (this->vehicles_[v1].getCost(adjMatrix_) + this->vehicles_[v2].getCost(adjMatrix_) < costBoth){
        this->vehicles_[v1].currLoad = currLoad1 - this->demands_[this->vehicles_[v2].route[client2]] + this->demands_[this->vehicles_[v1].route[client1]];
        this->vehicles_[v2].currLoad = currLoad2 - this->demands_[this->vehicles_[v1].route[client1]] + this->demands_[this->vehicles_[v2].route[client2]];
        this->modifiedRoutes_ = true;
        return true;
    }
    
    std::iter_swap(this->vehicles_[v1].route.begin() + client1, this->vehicles_[v2].route.begin() + client2);
    
    return false;
}

bool Solution::shiftNInRoute(Vehicle& vehicle, const int nElem){
    if (vehicle.route.size() < 2 || nElem < 1 || nElem > vehicle.route.size() - 1)
        return false;
    
    int cost = vehicle.getCost(this->adjMatrix_);
    
    int r1 = rand()%(vehicle.route.size() - nElem);
    
    Vehicle vehicle_aux = vehicle;
    
    std::rotate(vehicle_aux.route.begin(), vehicle_aux.route.begin() + nElem, vehicle_aux.route.end() - r1);
    
    if (vehicle_aux.getCost(this->adjMatrix_) < cost){
        vehicle.route = vehicle_aux.route;
        this->modifiedRoutes_ = true;
        return true;
    }
    
    return false;
}

bool Solution::swapOneInRoute(Vehicle& vehicle){
    std::size_t rsize = vehicle.route.size();
    if (rsize < 2)
        return false;
    
    int cost = vehicle.getCost(this->adjMatrix_);
    
    if (rsize == 2) {
        std::iter_swap(vehicle.route.begin(), vehicle.route.begin() + 1);
        if (vehicle.getCost(this->adjMatrix_) < cost)
            return true;
        std::iter_swap(vehicle.route.begin(), vehicle.route.begin() + 1);
        return false;
    }
    
    int r1 = rand()%rsize;
    int r2 = rand()%rsize;
    while (r1 == r2)
        r2 = rand()%rsize;
    
    std::iter_swap(vehicle.route.begin() + r1, vehicle.route.begin() + r2);
    
    if (vehicle.getCost(this->adjMatrix_) < cost)
        return true;
    
    std::iter_swap(vehicle.route.begin() + r1, vehicle.route.begin() + r2);
    return false;
}

// Heurística de Construção
void Solution::generateFirstSolution(RelaxationLevel level){
    std::vector< std::pair<int, int> > unvisitedClients;
    srand(time(0));
    
    for (int i = 1; i < this->adjMatrix_[0].size(); i++)
        unvisitedClients.push_back({i,this->demands_[i]});
    
    std::sort(unvisitedClients.begin(), unvisitedClients.end(), [](const auto a1, const auto a2){return a1.second > a2.second;});
    
    // Visit to first clients of each vehicle
    this->vehicles_.resize(this->nVehicles_);
    for(auto& vehicle : this->vehicles_){
        vehicle.route.push_back(unvisitedClients.front().first);
        vehicle.currLoad += this->demands_[unvisitedClients.front().first];
        unvisitedClients.erase(unvisitedClients.begin());
    }
    
    switch (level) {
        case Total:
            
            for (int i = 0; i < unvisitedClients.size(); i++){
                for(int j = 0; j < this->adjMatrix_.size(); j++) {
                    int vehicle = (rand()%this->nVehicles_);
                    int client = (rand()%(this->vehicles_[vehicle].route.size()));
                    if (this->vehicles_[vehicle].currLoad + this->demands_[unvisitedClients[i].first] <= this->capacity_) {
                        this->vehicles_[vehicle].route.insert(this->vehicles_[vehicle].route.begin() + client + 1, unvisitedClients[i].first);
                        this->vehicles_[vehicle].currLoad += this->demands_[unvisitedClients[i].first];
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
                        if (adjMatrix_[i][this->vehicles_[j].route[k]] < minDistance && vehicles_[j].currLoad + this->demands_[unvisitedClients[i].first] <= this->capacity_) {
                            minDistance = adjMatrix_[i][this->vehicles_[j].route[k]];
                            bestVehicleIndex = j;
                            bestClientIndex = k;
                        }
                    }
                }
                
                // Atualiza rota escolhida
                this->vehicles_[bestVehicleIndex].route.insert(this->vehicles_[bestVehicleIndex].route.begin() + bestClientIndex + 1, unvisitedClients[i].first);
                this->vehicles_[bestVehicleIndex].currLoad += this->demands_[unvisitedClients[i].first];
            }
            break;
        default:
            std::cerr << "Opção de relaxação inválida." << std::endl;
            break;
    }
    
    this->modifiedRoutes_ = true;
}

void Solution::printRoutes(){
    int i = 0;
    for (auto vehicle : this->vehicles_){
        std::cout << "Rota " << ++i << ": ";
        for (auto client: vehicle.route)
            std:: cout << client << " ";
        std::cout << "Carga: " << vehicle.currLoad << " Custo: " << vehicle.getCost(adjMatrix_) << std::endl;
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
    return (int) solution;
}

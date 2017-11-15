//
//  Solution.cpp
//  ProjetoFinal
//
//  Created by Pedro Figueirêdo on 14/11/2017.
//  Copyright © 2017 UFPB. All rights reserved.
//

#include "../Headers/Solution.hpp"

Solution::Solution(const std::vector< std::vector<float> >& adjMatrix, const std::vector<int>& demands, const unsigned nVehicles, const unsigned capacity):
modifiedRoutes_(true), currCost_(0), adjMatrix_(adjMatrix), demands_(demands),nVehicles_(nVehicles), capacity_(capacity){
    this->generateFirstSolution();
}

Solution::~Solution(){}

// Heurística de Construção
void Solution::generateFirstSolution(){
    std::vector< std::pair<int, float> > unvisitedClients;
    
    for (int i = 0; i < this->adjMatrix_[0].size(); i++)
        unvisitedClients.push_back({i,adjMatrix_[0][i]});
    
    std::sort(unvisitedClients.begin(), unvisitedClients.end(), [](const auto a1, const auto a2){return a1.second < a2.second;});
    
    unvisitedClients.erase(unvisitedClients.begin());
    
    // Visit to first clients of each vehicle
    this->vehicles_.resize(this->nVehicles_);
    for(auto& vehicle : this->vehicles_){
        vehicle.route.push_back(unvisitedClients.front().first);
        vehicle.currLoad += this->demands_[unvisitedClients.front().first];
        unvisitedClients.erase(unvisitedClients.begin());
    }
    
    for (int i = 0; i < unvisitedClients.size(); i++)
        unvisitedClients[i].second = demands_[unvisitedClients[i].first];
    
    
    std::sort(unvisitedClients.begin(), unvisitedClients.end(), [](const auto a1, const auto a2){return a1.second > a2.second;});
    
    for (int i = 0; i < unvisitedClients.size(); i++){
        int bestVehicleIndex = -1, bestClientIndex = -1;
        float minDistance = std::numeric_limits<float>::max();
        
        // Descobre qual o cliente já visitado mais próximo que cabe no veículo
        for(int j = 0; j < this->vehicles_.size(); j++){
            for (int k = 0; k < this->vehicles_[j].route.size(); k++){
                //std::cout << "Distância entre " << this->vehicles_[j].route[k] << " e " << unvisitedClients[i].first  << ": " << adjMatrix_[i][this->vehicles_[j].route[k]] << std::endl;
                //std::cout << "Peso do veículo: " << this->vehicles_[j].currLoad << " Demanda:" <<demands_[unvisitedClients[i].first] << std::endl;
                if (adjMatrix_[i][this->vehicles_[j].route[k]] < minDistance && vehicles_[j].currLoad + this->demands_[unvisitedClients[i].first] <= this->capacity_) {
                    minDistance = adjMatrix_[i][this->vehicles_[j].route[k]];
                    bestVehicleIndex = j;
                    bestClientIndex = k;
                }
            }
        }
        
        // Atualiza rota escolhida
        //std::cout << "Veículo escolhido: " << bestVehicleIndex << std::endl;
        this->vehicles_[bestVehicleIndex].route.insert(this->vehicles_[bestVehicleIndex].route.begin() + bestClientIndex + 1, unvisitedClients[i].first);
        this->vehicles_[bestVehicleIndex].currLoad += this->demands_[unvisitedClients[i].first];
    }
    
    this->modifiedRoutes_ = true;
}

void Solution::printRoutes(){
    int i = 0;
    for (auto vehicle : this->vehicles_){
        std::cout << "Rota " << ++i << ": ";
        for (auto client: vehicle.route)
            std:: cout << client << " ";
        std::cout << std::endl;
    }
}

int Solution::getCost(){
    // Avoids unecessary calculation
    if (!modifiedRoutes_)
        return this->currCost_;
    
    float solution = 0;
    
    for (auto vehicle : this->vehicles_){
        int i = 0;
        for (auto city : vehicle.route){
            solution += this->adjMatrix_[i][city];
            i = city;
        }
        solution += this->adjMatrix_[i][0];
    }
    
    return (int) solution;
}


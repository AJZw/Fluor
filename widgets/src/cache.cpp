/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-05-21
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "cache.h"

#include <QDebug>

namespace Cache {

/*
Constructor: Sets up the fluorophore cache. Ment as the central spil of all (inuse) fluorophore data.
    :param source: the (on the HDD) data object to request data from. CANNOT be a nullptr
*/
Cache::Cache(Data::Factory& factory, Data::Fluorophores& source) :
    counter(0),
    source_factory(factory),
    source_data(source),
    items(),
    data(20)
{

}

/*
Reserves space for a defined amount of entrees in the counter. Checks for integer overflow.
    :param size: the amount of entrees to reserves
    :returns: first valid count
*/
unsigned int Cache::getCounter(unsigned int size) {
    if(UINT_MAX - size < this->counter){
        // Add resetting of the counter value back to the minimum possible value in this->items and reset the value
        qFatal("Cache::getCounter: max counter value surpassed");
    }

    unsigned int current_counter = this->counter;
    this->counter += size;

    return current_counter;
}

/*
Print the internal state of the cache to the QDebug stream
*/
void Cache::printState() const {
    QDebug stream = qDebug();
    
    stream << "Cache::items:";

    for(CacheID item : this->items){
        stream << item;
    }

    stream << "\nCache::data:";

    for(auto key : this->data){
        stream << "{" << key.first << "}";
    }
}

/*
Sort a vector based on the SortOption
    :param input: the vector to sort
    :param option: the way to sort he vector
*/
void Cache::sortVector(std::vector<CacheID>& input, SortOption option){
    switch(option){
    case SortOption::Additive:{
        std::sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->index() < obj_b.data->index();
            }
        );
        return;
    }
    case SortOption::AdditiveReversed:{
        std::sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->index() > obj_b.data->index();
            }
        );
        return;
    }
    case SortOption::Alphabetical:{
        std::sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->name() < obj_b.data->name();
            }
        );
        return;
    }
    case SortOption::AlphabeticalReversed:{
        std::sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->name() > obj_b.data->name();
            }
        );
        return;
    }
    case SortOption::Emission:{
        // Presort alphabetically
        std::sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->name() < obj_b.data->name();
            }
        );

        // Then sort based on emission while keeping presort
        std::stable_sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->emissionMax() < obj_b.data->emissionMax();
            }
        );
        return;
    }
    case SortOption::EmissionReversed:{
        // Presort alphabetically
        std::sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->name() < obj_b.data->name();
            }
        );

        // Then sort based on emission while keeping presort
        std::stable_sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->emissionMax() > obj_b.data->emissionMax();
            }
        );
        return;
    }
    case SortOption::Excitation:{
        // Presort alphabetically
        std::sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->name() < obj_b.data->name();
            }
        );

        // Then sort based on emission while keeping presort
        std::stable_sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->excitationMax() < obj_b.data->excitationMax();
            }
        );
        return;
    }
    case SortOption::ExcitationReversed:{
         // Presort alphabetically
        std::sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->name() < obj_b.data->name();
            }
        );

        // Then sort based on emission while keeping presort
        std::stable_sort(input.begin(), input.end(), 
            [](const CacheID& obj_a, const CacheID& obj_b){
                return obj_a.data->excitationMax() > obj_b.data->excitationMax();
            }
        );
        return;
    }
    }
}

/*
Request a CacheSpectrum object of a specific id, if the id is already present just returns the pointer, if not builds the requested object first
    :param id: the fluorophore ID, if ID is not found, will return a valid but all 0 object (see Data::Fluorophore standard behavior)
    :returns: pointer to the CacheSpectrum object of the requested ID
*/
Data::CacheSpectrum* Cache::getData(const QString& id, const QString& name, const unsigned int counter){
    std::unordered_map<QString, Data::CacheSpectrum>::iterator spectrum = this->data.find(id);
    
    if(spectrum == this->data.end()){
        // Hash-miss so request spectrum data from HDD
        auto insert = this->data.insert(std::pair<QString, Data::CacheSpectrum>(id, this->source_data.getCacheSpectrum(this->source_factory, id, name, counter)));
    
        return &insert.first->second;
    }else{
        // Hash-hit, replace the name, and counter
        spectrum->second.setName(name);
        spectrum->second.setIndex(counter);
        return &spectrum->second;
    }
}

/*
Slot: receives fluorophores additions from the GUI. Adds the fluorophores ID to the items & data attributes
    :param fluorophores: fluorophores to add
*/
void Cache::cacheAdd(std::set<Data::FluorophoreID>& fluorophores){
    // Taking for granted that std::size_t is much bigger then unsigned int, but so be it, highly unlikely that you can ever add more then INT_MAX options in one go
    unsigned int current_counter = this->getCounter(static_cast<unsigned int>(fluorophores.size()));

    for(Data::FluorophoreID entree : fluorophores){
        std::pair<std::set<CacheID>::iterator, bool> cache_entree = this->items.emplace(entree.id, entree.name);

        if(cache_entree.second){
            // If succesfull, it is a new entree, so need to link the data pointer, and add the counter
            cache_entree.first->data = this->getData(entree.id, entree.name, entree.order + current_counter);
        }else{
            // If unsuccesfull, the entree already exists, no update needed
        }
    }

    this->printState();

    this->sync();
}

/*
Slot: receives fluorophores removals from the GUI. Removes the fluorophores ID from the items attributes.
    :param fluorophores: fluorophores to remove
*/
void Cache::cacheRemove(std::set<Data::FluorophoreID>& fluorophores){
    for(Data::FluorophoreID entree: fluorophores){
        CacheID id(entree.id, entree.name);
        this->items.erase(id);
    }

    this->printState();

    this->sync();
}

/*
Construct the necessary outputs for synchronization of the cache and GUI widgets
*/
void Cache::sync() {
    // The Fluor::LineEdit widgets require a sorted list of FluorophoreIDs & link to the CacheSpectrum data (for emission/excitation visibility stuff)

    // The plan:
    // transform set into vector
    // based on the sorting property sort the vector
    // emit sync signal

    // Convert items set into a vector (for custom ordering)
    std::vector<CacheID> fluor_output;
    fluor_output.reserve(this->items.size());

    fluor_output.insert(fluor_output.end(), this->items.begin(), this->items.end());

    // Sort based on sort qualifyer, when properly implemented this code has to request the sorting order somehow
    this->sortVector(fluor_output, SortOption::Additive);

    emit this->syncFluor(fluor_output);

    // The Graph widgets require a not-necessary sorted collection of pointer to CacheSpectrum data




}

} // Cache namespace

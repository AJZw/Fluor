/**** General **************************************************************
** Version:    v0.9.12
** Date:       2020-10-28
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "cache.h"

#include <QDebug>

namespace Cache {

/*
Constructor: Sets up the fluorophore cache. Ment as the central spill of all
(in-use) fluorophore data. :param factory: the data factory, connects and
provides safe access to the data files :param source: the fluorophore reader
provides access using the factory to the fluorophore spectra
*/
Cache::Cache(Data::Factory& factory, Data::FluorophoreReader& source)
    : source_factory(factory), source_data(source), items(), data(20), cache_settings() {}

/*
Reserves space for a defined amount of entrees in the counter. Checks for
integer overflow. :param size: the amount of entrees to reserves :returns: first
valid count
*/
unsigned int Cache::getCounter(unsigned int size) {
  if (UINT_MAX - size < this->counter) {
    qDebug() << "Cache::getCounter: counter overflow immenent, rebuilding counter";
    this->rebuildCounter();
  }

  unsigned int current_counter = this->counter;
  this->counter += size;

  return current_counter;
}

/*
The counter can only give a way a certain amount of index before overflowing.
Before that happens use this function to redistribute the indexes
*/
void Cache::rebuildCounter() {
  // Get the items, and redistribute the new index value
  std::vector<ID> cache_state;
  cache_state.reserve(this->items.size());
  cache_state.insert(cache_state.end(), this->items.begin(), this->items.end());

  std::sort(cache_state.begin(), cache_state.end(),
            [](const ID& obj_a, const ID& obj_b) { return obj_a.data->index() < obj_b.data->index(); });

  for (std::size_t i = 0; i < cache_state.size(); ++i) {
    cache_state[i].data->setIndex(static_cast<uint>(i));
  }

  this->counter = static_cast<uint>(cache_state.size());
}

/*
The cache can store a big amount of fluorophore data. To prevent the cache from
storing too many spectra, this function can be used to rebuild the cache to only
contain the currently used cache elements. If rebuild happens after
Cache::remove() without synchronisation with the GUI, this function can cause
dangling pointers.
*/
void Cache::rebuildCache() {
  // Get vector of keys in cache->data
  std::vector<QString> data_state;
  data_state.resize(this->data.size());

  std::transform(this->data.begin(), this->data.end(), data_state.begin(), [](const auto& pair) { return pair.first; });
  std::sort(data_state.begin(), data_state.end());

  // Get vector of keys in cache->items
  std::vector<QString> cache_state;
  cache_state.resize(this->items.size());
  std::transform(this->items.begin(), this->items.end(), cache_state.begin(), [](const auto& entree) { return entree.id; });
  std::sort(cache_state.begin(), cache_state.end());

  std::vector<QString> difference;
  difference.reserve(data_state.size());
  std::set_difference(data_state.begin(), data_state.end(), cache_state.begin(), cache_state.end(), std::back_inserter(difference));

  // Check if all indexes are in use, if so increase minimum size
  if (difference.size() == 0) {
    qDebug() << "Cache::rebuildCache: max size insufficient, increasing max size.";

    this->max_cache_size = static_cast<uint>(this->items.size()) + 10;
    return;
  }

  qDebug() << "Cache::rebuildCache: max cache size reached, removing" << difference.size() << "unused entrees.";

  for (const QString& entree : difference) {
    this->data.erase(this->data.find(entree));
  }

  // this->printState();
}

/*
Print the internal state of the cache to the QDebug stream
*/
void Cache::printState() const {
  QDebug stream = qDebug();

  stream << "Cache::items:";

  for (ID item : this->items) {
    stream << item;
  }

  stream << "\nCache::data:";

  for (auto key : this->data) {
    stream << "{" << key.first << "}";
  }
}

/*
Sort a vector based on the SortOption
  :param input: the vector to sort
  :param option: the way to sort he vector
*/
void Cache::sortVector(std::vector<ID>& input, State::SortMode mode) {
  switch (mode) {
    case State::SortMode::Additive: {
      std::sort(input.begin(), input.end(), [](const ID& obj_a, const ID& obj_b) { return obj_a.data->index() < obj_b.data->index(); });
      return;
    }
    case State::SortMode::AdditiveReversed: {
      std::sort(input.begin(), input.end(), [](const ID& obj_a, const ID& obj_b) { return obj_a.data->index() > obj_b.data->index(); });
      return;
    }
    case State::SortMode::Alphabetical: {
      std::sort(input.begin(), input.end(), [](const ID& obj_a, const ID& obj_b) { return obj_a.name < obj_b.name; });
      return;
    }
    case State::SortMode::AlphabeticalReversed: {
      std::sort(input.begin(), input.end(), [](const ID& obj_a, const ID& obj_b) { return obj_a.name > obj_b.name; });
      return;
    }
    case State::SortMode::Emission: {
      // Presort alphabetically
      std::sort(input.begin(), input.end(), [](const ID& obj_a, const ID& obj_b) { return obj_a.name < obj_b.name; });

      // Then sort based on emission while keeping presort
      std::stable_sort(input.begin(), input.end(),
                       [](const ID& obj_a, const ID& obj_b) { return obj_a.data->emissionMax() < obj_b.data->emissionMax(); });
      return;
    }
    case State::SortMode::EmissionReversed: {
      // Presort alphabetically
      std::sort(input.begin(), input.end(), [](const ID& obj_a, const ID& obj_b) { return obj_a.name < obj_b.name; });

      // Then sort based on emission while keeping presort
      std::stable_sort(input.begin(), input.end(),
                       [](const ID& obj_a, const ID& obj_b) { return obj_a.data->emissionMax() > obj_b.data->emissionMax(); });
      return;
    }
    case State::SortMode::Excitation: {
      // Presort alphabetically
      std::sort(input.begin(), input.end(), [](const ID& obj_a, const ID& obj_b) { return obj_a.name < obj_b.name; });

      // Then sort based on emission while keeping presort
      std::stable_sort(input.begin(), input.end(),
                       [](const ID& obj_a, const ID& obj_b) { return obj_a.data->excitationMax() < obj_b.data->excitationMax(); });
      return;
    }
    case State::SortMode::ExcitationReversed: {
      // Presort alphabetically
      std::sort(input.begin(), input.end(), [](const ID& obj_a, const ID& obj_b) { return obj_a.name < obj_b.name; });

      // Then sort based on emission while keeping presort
      std::stable_sort(input.begin(), input.end(),
                       [](const ID& obj_a, const ID& obj_b) { return obj_a.data->excitationMax() > obj_b.data->excitationMax(); });
      return;
    }
  }
}

/*
Request a CacheSpectrum object of a specific id, if the id is already present
just returns the pointer, if not builds the requested object first :param id:
the fluorophore ID, if ID is not found, will return a valid but all 0 object
(see Data::Fluorophore standard behavior) :returns: pointer to the CacheSpectrum
object of the requested ID
*/
Data::CacheSpectrum* Cache::getData(const QString& id, const unsigned int counter) {
  std::unordered_map<QString, Data::CacheSpectrum>::iterator spectrum = this->data.find(id);

  if (spectrum == this->data.end()) {
    // Hash-miss so request spectrum data from HDD
    auto insert = this->data.insert(std::pair<QString, Data::CacheSpectrum>(id, this->source_data.getCacheSpectrum(id, counter)));

    // Set visibility to default
    insert.first->second.setVisibleExcitation(this->cache_settings.visible_excitation);
    insert.first->second.setVisibleEmission(this->cache_settings.visible_emission);

    return &insert.first->second;
  } else {
    // Hash-hit, replace the counter
    spectrum->second.setIndex(counter);
    // Reset to default
    spectrum->second.setVisibleExcitation(this->cache_settings.visible_excitation);
    spectrum->second.setVisibleEmission(this->cache_settings.visible_emission);
    return &spectrum->second;
  }
}

/*
Slot: receives fluorophores additions from the GUI. Adds the fluorophores ID to
the items & data attributes :param fluorophores: fluorophores to add
*/
void Cache::add(std::vector<Data::FluorophoreID>& fluorophores) {
  // Taking for granted that std::size_t is much bigger then unsigned int, but
  // so be it, highly unlikely that you ever add more then INT_MAX options in
  // one go
  unsigned int current_counter = this->getCounter(static_cast<unsigned int>(fluorophores.size()));

  for (const Data::FluorophoreID& entree : fluorophores) {
    std::pair<std::set<ID>::iterator, bool> cache_entree = this->items.emplace(entree.id, entree.name);

    if (cache_entree.second) {
      // If succesfull, it is a new entree, so need to link the data pointer,
      // and add the counter
      cache_entree.first->data = this->getData(entree.id, entree.order + current_counter);

    } else {
      // If unsuccesfull, the entree already exists, no update needed
    }
  }

  // this->printState();
}

/*
Slot: receives fluorophores removals from the GUI. Removes the fluorophores ID
from the items attributes. :param fluorophores: fluorophores to remove
*/
void Cache::remove(std::vector<Data::FluorophoreID>& fluorophores) {
  for (const Data::FluorophoreID& entree : fluorophores) {
    ID id(entree.id, entree.name);
    this->items.erase(id);
  }

  // this->printState();
}

/*
Construct a cache state representation for synchronisation with the GUI
*/
const std::vector<ID> Cache::state() const {
  // Convert items set into a vector (for custom ordering)
  std::vector<ID> cache_state;
  cache_state.reserve(this->items.size());
  cache_state.insert(cache_state.end(), this->items.begin(), this->items.end());

  // Sort based on sort qualifyer
  this->sortVector(cache_state, this->cache_settings.sort_mode);

  return cache_state;
}

/*
Slot: Sets the cache state and sync these changes
  :param state: the new cache state
*/
void Cache::setSettings(Settings settings) {
  this->cache_settings = settings;

  for (auto& item : this->items) {
    item.data->setVisibleExcitation(this->cache_settings.visible_excitation);
    item.data->setVisibleEmission(this->cache_settings.visible_emission);
  }
}

/*
Slot: set the cache state excitation visibility and updates this change to all
loaded spectra. Updates afterwards :param visible: visibility of the excitation
*/
void Cache::setSettingsExcitation(bool visible) {
  this->cache_settings.visible_excitation = visible;

  for (auto& item : this->items) {
    item.data->setVisibleExcitation(visible);
  }
}

/*
Slot: set the cache state emission visibility and updates this change to all
loaded spectra. Updates afterwards :param visible: visibility of the emission
*/
void Cache::setSettingsEmission(bool visible) {
  this->cache_settings.visible_emission = visible;

  for (auto& item : this->items) {
    item.data->setVisibleEmission(visible);
  }
}

/*
Slot: set the cache state sorting option and sync's this.
  :param option: the new sorting option
*/
void Cache::setSettingsSorting(State::SortMode mode) {
  if (mode == this->cache_settings.sort_mode) {
    return;
  }

  this->cache_settings.sort_mode = mode;
}

}  // namespace Cache

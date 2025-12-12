#pragma once
#include "TariffClasses.h"
#include <vector>
#include <memory>
#include <string>

class RailwayStation {
private:
    std::vector<std::shared_ptr<ITariffStrategy>> tariffs;

public:
    ~RailwayStation() = default;

    void addRegularTariff(const std::string& destination, double price);
    void addDiscountTariff(const std::string& destination, double price, double discount);
    void removeTariff(int index);
    void updateTariff(int index, const std::string& destination, double price, double discount = 0);

    std::shared_ptr<ITariffStrategy> findMinCostDestination() const;
    void sortByDestination();
    void sortByFinalPrice();

    size_t getTariffCount() const { return tariffs.size(); }
    std::shared_ptr<ITariffStrategy> getTariff(int index) const;

    std::vector<std::string> getAllTariffsInfo() const;
    std::string getStatistics() const;

    void clearAllTariffs() { tariffs.clear(); }

    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
};
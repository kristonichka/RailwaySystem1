#include "RailwayStation.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>  
#include <stdexcept>

void RailwayStation::addRegularTariff(const std::string& destination, double price) {
    if (destination.empty()) {
        throw std::invalid_argument("Направление не может быть пустым");
    }
    if (price <= 0) {
        throw std::invalid_argument("Цена должна быть положительной");
    }
    tariffs.push_back(std::make_shared<RegularTariff>(destination, price));
}

void RailwayStation::addDiscountTariff(const std::string& destination, double price, double discount) {
    if (destination.empty()) {
        throw std::invalid_argument("Направление не может быть пустым");
    }
    if (price <= 0) {
        throw std::invalid_argument("Цена должна быть положительной");
    }
    if (discount < 1 || discount > 99) {
        throw std::invalid_argument("Скидка должна быть в диапазоне 1-99%");
    }
    tariffs.push_back(std::make_shared<DiscountTariff>(destination, price, discount));
}

void RailwayStation::removeTariff(int index) {
    if (index < 0 || index >= static_cast<int>(tariffs.size())) {
        throw std::out_of_range("Неверный индекс");
    }
    tariffs.erase(tariffs.begin() + index);
}

void RailwayStation::updateTariff(int index, const std::string& destination, double price, double discount) {
    if (index < 0 || index >= static_cast<int>(tariffs.size())) {
        throw std::out_of_range("Неверный индекс");
    }

    removeTariff(index);
    if (discount > 0) {
        addDiscountTariff(destination, price, discount);
    }
    else {
        addRegularTariff(destination, price);
    }
}

std::shared_ptr<ITariffStrategy> RailwayStation::findMinCostDestination() const {
    if (tariffs.empty()) {
        throw std::runtime_error("Нет тарифов для поиска");
    }

    auto minTariff = std::min_element(tariffs.begin(), tariffs.end(),
        [](const std::shared_ptr<ITariffStrategy>& a,
            const std::shared_ptr<ITariffStrategy>& b) {
                return a->getFinalPrice() < b->getFinalPrice();
        });

    return *minTariff;
}

void RailwayStation::sortByDestination() {
    std::sort(tariffs.begin(), tariffs.end(),
        [](const std::shared_ptr<ITariffStrategy>& a,
            const std::shared_ptr<ITariffStrategy>& b) {
                return a->getDestination() < b->getDestination();
        });
}

void RailwayStation::sortByFinalPrice() {
    std::sort(tariffs.begin(), tariffs.end(),
        [](const std::shared_ptr<ITariffStrategy>& a,
            const std::shared_ptr<ITariffStrategy>& b) {
                return a->getFinalPrice() < b->getFinalPrice();
        });
}

std::shared_ptr<ITariffStrategy> RailwayStation::getTariff(int index) const {
    if (index < 0 || index >= static_cast<int>(tariffs.size())) {
        throw std::out_of_range("Неверный индекс");
    }
    return tariffs[index];
}

std::vector<std::string> RailwayStation::getAllTariffsInfo() const {
    std::vector<std::string> result;
    for (size_t i = 0; i < tariffs.size(); ++i) {
        std::ostringstream oss;
        oss << (i + 1) << ". " << tariffs[i]->getInfoString();
        result.push_back(oss.str());
    }
    return result;
}

std::string RailwayStation::getStatistics() const {
    if (tariffs.empty()) {
        return "Нет данных для статистики";
    }

    int regularCount = 0;
    int discountCount = 0;
    double totalBasePrice = 0.0;
    double totalFinalPrice = 0.0;

    for (const auto& tariff : tariffs) {
        if (tariff->getTariffType() == "Обычный") {
            regularCount++;
        }
        else {
            discountCount++;
        }
        totalBasePrice += tariff->getBasePrice();
        totalFinalPrice += tariff->getFinalPrice();
    }

    std::ostringstream oss;
    oss << "Статистика:\n"
        << "Всего тарифов: " << tariffs.size() << "\n"
        << "Обычных: " << regularCount << "\n"
        << "Со скидкой: " << discountCount << "\n"
        << "Суммарная базовая стоимость: "
        << std::fixed << std::setprecision(2) << totalBasePrice << " руб.\n"
        << "Суммарная стоимость со скидками: "
        << std::fixed << std::setprecision(2) << totalFinalPrice << " руб.";

    return oss.str();
}

bool RailwayStation::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    for (const auto& tariff : tariffs) {
        file << tariff->getDestination() << ";"
            << tariff->getBasePrice() << ";"
            << tariff->getTariffType() << ";";

        if (auto discountTariff = std::dynamic_pointer_cast<DiscountTariff>(tariff)) {
            file << discountTariff->getDiscountPercent();
        }
        else {
            file << "0";
        }
        file << "\n";
    }

    file.close();
    return true;
}

bool RailwayStation::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    tariffs.clear();
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string destination, type;
        double price, discount;
        char delimiter;

        if (std::getline(iss, destination, ';')) {
            iss >> price >> delimiter;
            std::getline(iss, type, ';');
            iss >> discount;

            if (type == "Со скидкой" && discount > 0) {
                addDiscountTariff(destination, price, discount);
            }
            else {
                addRegularTariff(destination, price);
            }
        }
    }

    file.close();
    return true;
}
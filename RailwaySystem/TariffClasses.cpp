#include "TariffClasses.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

ITariffStrategy::ITariffStrategy(std::string dest, double price)
    : destination(std::move(dest)), basePrice(price) {
    if (basePrice < 0) {
        throw std::invalid_argument("Цена не может быть отрицательной");
    }
}

std::string ITariffStrategy::getInfoString() const {
    std::ostringstream oss;
    oss << "Направление: " << destination
        << ", Базовая цена: " << std::fixed << std::setprecision(2)
        << basePrice << " руб.";
    return oss.str();
}

RegularTariff::RegularTariff(const std::string& dest, double price)
    : ITariffStrategy(dest, price) {
}

double RegularTariff::getFinalPrice() const {
    return basePrice;
}

std::string RegularTariff::getTariffType() const {
    return "Обычный";
}

DiscountTariff::DiscountTariff(const std::string& dest, double price, double discount)
    : ITariffStrategy(dest, price), discountPercent(discount) {
    if (discount < 1 || discount > 99) {
        throw std::invalid_argument("Скидка должна быть в диапазоне 1-99%");
    }
}

double DiscountTariff::getFinalPrice() const {
    double discountMultiplier = 1.0 - (discountPercent / 100.0);
    return basePrice * discountMultiplier;
}

std::string DiscountTariff::getTariffType() const {
    return "Со скидкой";
}

std::string DiscountTariff::getInfoString() const {
    std::ostringstream oss;
    oss << ITariffStrategy::getInfoString()
        << ", Скидка: " << std::fixed << std::setprecision(1)
        << discountPercent << "%, Итоговая цена: "
        << std::setprecision(2) << getFinalPrice()
        << " руб., Тип: Тариф со скидкой";
    return oss.str();
}
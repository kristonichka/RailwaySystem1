#pragma once
#include <string>
#include <memory>

// Интерфейс стратегии для тарифов
class ITariffStrategy {
protected:
    std::string destination;
    double basePrice;

public:
    ITariffStrategy(std::string dest, double price);
    virtual ~ITariffStrategy() = default;

    virtual double getFinalPrice() const = 0;
    virtual std::string getTariffType() const = 0;
    const std::string& getDestination() const { return destination; }
    double getBasePrice() const { return basePrice; }

    virtual std::string getInfoString() const;
};

// Обычный тариф без скидки
class RegularTariff : public ITariffStrategy {
public:
    RegularTariff(const std::string& dest, double price);
    double getFinalPrice() const override;
    std::string getTariffType() const override;
};

// Тариф со скидкой
class DiscountTariff : public ITariffStrategy {
private:
    double discountPercent;

public:
    DiscountTariff(const std::string& dest, double price, double discount);
    double getFinalPrice() const override;
    std::string getTariffType() const override;
    double getDiscountPercent() const { return discountPercent; }
    std::string getInfoString() const override;
};
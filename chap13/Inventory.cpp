#include "Inventory.h"

Inventory::Inventory()
    : itemNumber(0), name(""), quantity(0), wholesaleCost(0.0), inventoryTotalCost(0.0) {
}

Inventory::Inventory(int number, double itemCost, int itemQuantity)
    : itemNumber(0), name(""), quantity(0), wholesaleCost(0.0), inventoryTotalCost(0.0) {
    setItemNumber(number);
    setCost(itemCost);
    setQuantity(itemQuantity);
    setTotalCost();
}

Inventory::Inventory(int number, const std::string& itemName, double itemCost, int itemQuantity)
    : itemNumber(0), name(""), quantity(0), wholesaleCost(0.0), inventoryTotalCost(0.0) {
    setItemNumber(number);
    setName(itemName);
    setCost(itemCost);
    setQuantity(itemQuantity);
    setTotalCost();
}

void Inventory::setItemNumber(int n) {
    itemNumber = (n >= 0) ? n : 0;
}

void Inventory::setName(const std::string& itemName) {
    name = itemName;
}

void Inventory::setQuantity(int q) {
    quantity = (q >= 0) ? q : 0;
    setTotalCost();
}

void Inventory::setCost(double c) {
    wholesaleCost = (c >= 0.0) ? c : 0.0;
    setTotalCost();
}

void Inventory::setTotalCost() {
    inventoryTotalCost = quantity * wholesaleCost;
}

int Inventory::getItemNumber() const {
    return itemNumber;
}

const std::string& Inventory::getName() const {
    return name;
}

int Inventory::getQuantity() const {
    return quantity;
}

double Inventory::getCost() const {
    return wholesaleCost;
}

double Inventory::getTotalCost() const {
    return inventoryTotalCost;
}

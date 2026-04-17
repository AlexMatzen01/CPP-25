#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>

class Inventory {
private:
    int itemNumber;
    std::string name;
    int quantity;
    double wholesaleCost;
    double inventoryTotalCost;

public:
    Inventory();
    Inventory(int number, double itemCost, int itemQuantity);
    Inventory(int number, const std::string& itemName, double itemCost, int itemQuantity);

    void setItemNumber(int n);
    void setName(const std::string& itemName);
    void setQuantity(int q);
    void setCost(double c);
    void setTotalCost();

    int getItemNumber() const;
    const std::string& getName() const;
    int getQuantity() const;
    double getCost() const;
    double getTotalCost() const;
};

#endif

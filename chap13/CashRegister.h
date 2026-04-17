#ifndef CASHREGISTER_H
#define CASHREGISTER_H

#include "Inventory.h"

class CashRegister {
private:
    Inventory* selectedItem;
    int purchaseQuantity;
    double unitWholesaleCost;
    double markupRate;
    double subTotal;
    double tax;
    double total;

    static constexpr double SALES_TAX_RATE = 0.06;

    void recalculateTotals();

public:
    CashRegister(Inventory& inventoryItem, int quantity, double markupRate = 0.30);

    bool setPurchaseQuantity(int quantity);
    int getPurchaseQuantity() const;
    double getCost() const;
    double getMarkupRate() const;
    double getRetailUnitPrice() const;
    double getSubTotal() const;
    double getTax() const;
    double getTotal() const;

    bool completePurchase();
};

#endif

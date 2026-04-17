#include "CashRegister.h"

CashRegister::CashRegister(Inventory& inventoryItem, int quantity, double markupRate)
    : selectedItem(&inventoryItem), purchaseQuantity(0), unitWholesaleCost(inventoryItem.getCost()), markupRate(0.30),
      subTotal(0.0), tax(0.0), total(0.0) {
    if (markupRate >= 0.0) {
        this->markupRate = markupRate;
    }

    setPurchaseQuantity(quantity);
}

bool CashRegister::setPurchaseQuantity(int quantity) {
    if (quantity <= 0 || quantity > selectedItem->getQuantity()) {
        return false;
    }

    purchaseQuantity = quantity;
    recalculateTotals();
    return true;
}

int CashRegister::getPurchaseQuantity() const {
    return purchaseQuantity;
}

double CashRegister::getCost() const {
    return unitWholesaleCost;
}

double CashRegister::getMarkupRate() const {
    return markupRate;
}

double CashRegister::getRetailUnitPrice() const {
    return unitWholesaleCost * (1.0 + markupRate);
}

double CashRegister::getSubTotal() const {
    return subTotal;
}

double CashRegister::getTax() const {
    return tax;
}

double CashRegister::getTotal() const {
    return total;
}

bool CashRegister::completePurchase() {
    if (purchaseQuantity <= 0 || purchaseQuantity > selectedItem->getQuantity()) {
        return false;
    }

    selectedItem->setQuantity(selectedItem->getQuantity() - purchaseQuantity);
    return true;
}

void CashRegister::recalculateTotals() {
    subTotal = getRetailUnitPrice() * purchaseQuantity;
    tax = subTotal * SALES_TAX_RATE;
    total = subTotal + tax;
}

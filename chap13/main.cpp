#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "CashRegister.h"
#include "Inventory.h"

namespace {

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

double calculateRetailPrice(double wholesaleCost, double markupRate) {
    return wholesaleCost * (1.0 + markupRate);
}

void printInventoryTable(const std::vector<Inventory>& inventoryItems, double markupRate) {
    std::cout << "\n========================== STORE INVENTORY ============================\n";
    std::cout << std::left << std::setw(12) << "Item #"
              << std::setw(20) << "Name"
              << std::right << std::setw(12) << "Price"
              << std::setw(14) << "Quantity" << '\n';
    std::cout << "---------------------------------------------------------------------\n";

    for (const Inventory& item : inventoryItems) {
        const double retailPrice = calculateRetailPrice(item.getCost(), markupRate);
        std::cout << std::left << std::setw(12) << item.getItemNumber()
                  << std::setw(20) << item.getName()
                  << std::right << std::setw(12) << std::fixed << std::setprecision(2)
                  << retailPrice
                  << std::setw(14) << item.getQuantity() << '\n';
    }

    std::cout << "=====================================================================\n";
}

Inventory* selectItem(std::vector<Inventory>& inventoryItems) {
    while (true) {
        std::cout << "\nEnter the item number you want to buy: ";
        int selectedNumber = 0;
        bool itemMatched = false;

        if (!(std::cin >> selectedNumber)) {
            std::cout << "Please enter a valid item number.\n";
            clearInputBuffer();
            continue;
        }

        // Find the item in stock that matches the user's selection.
        for (Inventory& item : inventoryItems) {
            if (item.getItemNumber() == selectedNumber) {
                itemMatched = true;
                if (item.getQuantity() <= 0) {
                    std::cout << "That item is currently out of stock. Choose another item.\n";
                    break;
                }

                return &item;
            }
        }

        if (!itemMatched) {
            std::cout << "Item number not found. Pick one from the table above.\n";
        }
    }
}

int requestPurchaseQuantity(const Inventory& item) {
    while (true) {
        std::cout << "How many would you like (1 - " << item.getQuantity() << "): ";
        int requestedQuantity = 0;

        if (!(std::cin >> requestedQuantity)) {
            std::cout << "Please enter a whole number.\n";
            clearInputBuffer();
            continue;
        }

        if (requestedQuantity <= 0) {
            std::cout << "Quantity must be at least 1.\n";
            continue;
        }

        // Guard against selling more than we currently have.
        if (requestedQuantity > item.getQuantity()) {
            std::cout << "Only " << item.getQuantity() << " in stock. Try a smaller number.\n";
            continue;
        }

        return requestedQuantity;
    }
}

void printReceipt(const Inventory& selectedItem,
                  int purchasedQuantity,
                  double retailUnitPrice,
                  const CashRegister& transaction,
                  double markupRate,
                  double salesTaxRate) {
    const int markupPercent = static_cast<int>(markupRate * 100 + 0.5);
    const int taxPercent = static_cast<int>(salesTaxRate * 100 + 0.5);

    std::cout << "\n============================= RECEIPT ================================\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(30) << "Item:"
              << selectedItem.getName() << " (#" << selectedItem.getItemNumber() << ")\n";
    std::cout << std::left << std::setw(30) << "Quantity Purchased:"
              << purchasedQuantity << '\n';
    std::cout << std::left << std::setw(30)
              << ("Unit Price (" + std::to_string(markupPercent) + "% markup):")
              << "$" << std::right << std::setw(10) << retailUnitPrice << '\n';
    std::cout << std::left << std::setw(30) << "Subtotal (before tax):"
              << "$" << std::right << std::setw(10) << transaction.getSubTotal() << '\n';
    std::cout << std::left << std::setw(30)
              << ("Sales Tax (" + std::to_string(taxPercent) + "%):")
              << "$" << std::right << std::setw(10) << transaction.getTax() << '\n';
    std::cout << std::left << std::setw(30) << "Total:"
              << "$" << std::right << std::setw(10) << transaction.getTotal() << '\n';
    std::cout << "=====================================================================\n";
}

}  // namespace

int main() {
    constexpr double MARKUP_RATE = 0.30;
    constexpr double SALES_TAX_RATE = 0.06;

    // Sample store inventory (wholesale costs) used for this checkout session.
    std::vector<Inventory> inventoryItems = {
        Inventory(1001, "Notebook", 1.25, 80),
        Inventory(1002, "Pencil", 0.40, 200),
        Inventory(1003, "Backpack", 12.50, 25),
        Inventory(1004, "Water Bottle", 6.20, 40),
        Inventory(1005, "Calculator", 15.00, 18)
    };

    printInventoryTable(inventoryItems, MARKUP_RATE);

    Inventory* selectedItem = selectItem(inventoryItems);
    const int requestedQuantity = requestPurchaseQuantity(*selectedItem);
    const double retailUnitPrice = calculateRetailPrice(selectedItem->getCost(), MARKUP_RATE);

    CashRegister transaction(*selectedItem, requestedQuantity, MARKUP_RATE);
    if (!transaction.completePurchase()) {
        std::cout << "Could not complete the purchase because the quantity was invalid.\n";
        return 1;
    }

    printReceipt(*selectedItem,
                 requestedQuantity,
                 retailUnitPrice,
                 transaction,
                 MARKUP_RATE,
                 SALES_TAX_RATE);
    printInventoryTable(inventoryItems, MARKUP_RATE);

    return 0;
}

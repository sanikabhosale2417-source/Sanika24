#ifndef INVENTORYMANAGER_H
#define INVENTORYMANAGER_H

/**
 * InventoryManager  –  C++ façade over the C inventory backend.
 *
 * Each method handles user interaction (prompts, validation, display)
 * and delegates storage work to the C functions in inventory.h / inventory.c.
 *
 * STL usage:
 *   std::vector<Item>  – holds the item list returned by list_items()
 *   std::sort          – sorts the vector before printing
 */
class InventoryManager {
public:
    void addItem();
    void viewItem();
    void updateItem();
    void deleteItem();
    void listItems();
};

#endif /* INVENTORYMANAGER_H */

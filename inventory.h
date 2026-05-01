#ifndef INVENTORY_H
#define INVENTORY_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NAME_LEN 40
#define INVENTORY_FILE "inventory.dat"

typedef struct {
    int   id;
    char  name[MAX_NAME_LEN];
    int   quantity;
    float price;
    int   is_deleted;   /* 0 = active, 1 = soft-deleted */
} Item;

/**
 * Add a new item.  Returns 1 on success, 0 if the ID already exists.
 */
int add_item(const Item item);

/**
 * Find an active item by ID and copy it into *out.
 * Returns 1 on success, 0 if not found / deleted.
 */
int get_item(int id, Item *out);

/**
 * Overwrite the record that matches id with the data in updated.
 * Returns 1 on success, 0 if not found / deleted.
 */
int update_item(int id, const Item *updated);

/**
 * Soft-delete the item with the given ID (sets is_deleted = 1).
 * Returns 1 on success, 0 if not found / already deleted.
 */
int delete_item(int id);

/**
 * Copy up to max_items active items into buffer[].
 * Returns the number of items actually copied.
 */
int list_items(Item *buffer, int max_items);

#ifdef __cplusplus
}
#endif

#endif /* INVENTORY_H */

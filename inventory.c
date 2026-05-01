/*
 * inventory.c  –  C backend for the Hybrid Inventory Manager
 *
 * Storage format: flat binary file of Item records.
 * Every record is the same size (sizeof(Item)), so the offset of
 * record number N is simply  N * sizeof(Item).
 * Deletion is a soft-delete: we flip is_deleted = 1 in place.
 */

#include "inventory.h"

#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Internal helpers                                                     */
/* ------------------------------------------------------------------ */

/**
 * Search for a record with the given id.
 * If found, stores its zero-based index in *index_out and copies
 * the record into *item_out (may be NULL if caller does not need it).
 * Returns 1 if found, 0 if not.
 * fp must be opened for reading.
 */
static int find_record(FILE *fp, int id, long *index_out, Item *item_out)
{
    if (fseek(fp, 0, SEEK_SET) != 0)
        return 0;

    Item tmp;
    long idx = 0;
    while (fread(&tmp, sizeof(Item), 1, fp) == 1) {
        if (tmp.id == id) {
            if (index_out) *index_out = idx;
            if (item_out)  *item_out  = tmp;
            return 1;
        }
        idx++;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

int add_item(const Item item)
{
    /* ---- check for duplicate id ----------------------------------- */
    FILE *fp = fopen(INVENTORY_FILE, "rb");
    if (fp) {
        Item existing;
        long dummy;
        int dup = find_record(fp, item.id, &dummy, &existing);
        fclose(fp);
        if (dup)
            return 0;   /* duplicate id */
    }

    /* ---- append new record --------------------------------------- */
    fp = fopen(INVENTORY_FILE, "ab");
    if (!fp)
        return 0;

    size_t written = fwrite(&item, sizeof(Item), 1, fp);
    fclose(fp);
    return (written == 1) ? 1 : 0;
}

int get_item(int id, Item *out)
{
    FILE *fp = fopen(INVENTORY_FILE, "rb");
    if (!fp)
        return 0;

    Item tmp;
    long dummy;
    int found = find_record(fp, id, &dummy, &tmp);
    fclose(fp);

    if (!found || tmp.is_deleted)
        return 0;

    if (out) *out = tmp;
    return 1;
}

int update_item(int id, const Item *updated)
{
    /* We need read + write access without truncating */
    FILE *fp = fopen(INVENTORY_FILE, "r+b");
    if (!fp)
        return 0;

    long idx;
    Item existing;
    if (!find_record(fp, id, &idx, &existing) || existing.is_deleted) {
        fclose(fp);
        return 0;
    }

    /* Seek to the start of that record and overwrite */
    long offset = idx * (long)sizeof(Item);
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return 0;
    }

    size_t written = fwrite(updated, sizeof(Item), 1, fp);
    fclose(fp);
    return (written == 1) ? 1 : 0;
}

int delete_item(int id)
{
    FILE *fp = fopen(INVENTORY_FILE, "r+b");
    if (!fp)
        return 0;

    long idx;
    Item existing;
    if (!find_record(fp, id, &idx, &existing) || existing.is_deleted) {
        fclose(fp);
        return 0;
    }

    existing.is_deleted = 1;

    long offset = idx * (long)sizeof(Item);
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return 0;
    }

    size_t written = fwrite(&existing, sizeof(Item), 1, fp);
    fclose(fp);
    return (written == 1) ? 1 : 0;
}

int list_items(Item *buffer, int max_items)
{
    if (!buffer || max_items <= 0)
        return 0;

    FILE *fp = fopen(INVENTORY_FILE, "rb");
    if (!fp)
        return 0;

    int count = 0;
    Item tmp;
    while (count < max_items && fread(&tmp, sizeof(Item), 1, fp) == 1) {
        if (!tmp.is_deleted)
            buffer[count++] = tmp;
    }

    fclose(fp);
    return count;
}

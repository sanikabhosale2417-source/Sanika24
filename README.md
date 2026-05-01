# Hybrid Inventory Manager

A console-based inventory manager demonstrating C/C++ interoperability:

- **C backend** (`inventory.c`) – binary file I/O with `fread` / `fwrite` / `fseek`
- **C++ frontend** (`InventoryManager.cpp`, `main.cpp`) – class, `std::vector`, `std::sort`

Data persists across restarts in `inventory.dat` (binary, same directory as executable).

---

## File Structure

```
inventory_project/
├── include/
│   ├── inventory.h          # C struct + extern "C" API declarations
│   └── InventoryManager.h   # C++ class declaration
├── src/
│   ├── inventory.c          # C backend (file I/O)
│   ├── InventoryManager.cpp # C++ class implementation
│   └── main.cpp             # Menu loop / entry point
├── Makefile
├── CMakeLists.txt
└── README.md
```

---

## Build & Run

### Option A – Make (recommended)

```bash
# Build
make

# Run
./inventory_manager

# Or build + run in one step
make run

# Clean all build artefacts and the data file
make clean
```

### Option B – CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
./inventory_manager
```

Requirements: GCC / G++ 7+ (or Clang 6+), GNU Make or CMake ≥ 3.14.

---

## Menu Overview

```
1. Add item     – prompt for ID, name, quantity, price; rejects duplicate IDs
2. View item    – look up one item by ID
3. Update item  – overwrite name/quantity/price for an existing ID
4. Delete item  – soft-delete (marked in file, invisible to view/list)
5. List all     – show all active items sorted by ID
6. Exit
```

All input is validated; the app re-prompts rather than crashing on bad input.

---

## Test Cases

- **TC-1 – Persist across restarts**
  Added items 1 (Widget, qty 10, $1.99), 2 (Gadget, qty 5, $9.99), 3 (Doohickey, qty 0, $0.50) then exited. Restarted and chose "List all" — all three items appeared with correct values. ✅

- **TC-2 – Duplicate ID rejected**
  With item 1 already in the file, attempted to add another item with ID 1. The app printed "Failed – ID 1 already exists" and no second record was written. ✅

- **TC-3 – Update persists**
  Updated item 2 (Gadget) to name "Super Gadget", quantity 20, price $14.99. Exited and restarted. "View item" for ID 2 showed the updated values. ✅

- **TC-4 – Deleted item invisible**
  Deleted item 3 (Doohickey). "List all" showed only items 1 and 2. "View item" for ID 3 reported "not found or has been deleted." Exited, restarted — same behaviour. ✅

- **TC-5 – Input validation**
  Entered `-5` for ID → re-prompted. Entered `-1` for quantity → re-prompted. Entered `abc` for price → re-prompted. Left name blank → re-prompted. Typed garbage at the main menu → re-prompted. None of these caused a crash. ✅

---

## Design Notes

| Concern | Handled by |
|---|---|
| Binary persistence | `fread` / `fwrite` in `inventory.c` |
| In-place update / delete | `fseek` to `record_index × sizeof(Item)` |
| Duplicate detection | Linear scan on add |
| Soft delete | `is_deleted` flag; filtered in `list_items()` and `get_item()` |
| STL vector | `std::vector<Item>` in `InventoryManager::listItems()` |
| STL sort | `std::sort` by `id` before printing |
| C/C++ linkage | `extern "C"` block in `inventory.h` |

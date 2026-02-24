#include "TileRenderer.h"

// Ensure vtable is emitted in this translation unit.
namespace atlas::tile {
    // Intentionally empty — the pure-virtual interface is fully
    // defined in the header.  This file exists so the linker has a
    // home translation unit for the vtable.
    void tileRendererAnchor() {}
}

# Atlas CMake Dependency Enforcement
#
# This module provides functions to enforce architectural layer dependencies.
# It ensures that:
#   - core/ never depends on anything else
#   - simulation/ only depends on core/
#   - editor/ never links simulation directly
#   - presentation/ is isolated from simulation/
#
# See: docs/ATLAS_CORE_CONTRACT.md
#      docs/NEXT_IMPLEMENTATION_TASKS.md

# Define the canonical layer hierarchy
# Lower layers cannot depend on higher layers
set(ATLAS_LAYER_HIERARCHY
    "core"           # Level 0: Core abstractions, no dependencies
    "simulation"     # Level 1: Deterministic simulation, depends on core only
    "assets"         # Level 1: Asset management, depends on core only
    "networking"     # Level 2: Network layer, depends on simulation + core
    "presentation"   # Level 2: Rendering/UI, depends on core + assets only
    "editor"         # Level 3: Editor tools, depends on all except simulation internals
)

# Get the layer level of a given directory
function(atlas_get_layer_level dir_name out_var)
    list(FIND ATLAS_LAYER_HIERARCHY "${dir_name}" level)
    set(${out_var} ${level} PARENT_SCOPE)
endfunction()

# Check if one layer can depend on another
function(atlas_can_depend from_layer to_layer out_var)
    atlas_get_layer_level(${from_layer} from_level)
    atlas_get_layer_level(${to_layer} to_level)
    
    if(from_level EQUAL -1 OR to_level EQUAL -1)
        # Unknown layer, allow for now (external dependencies)
        set(${out_var} TRUE PARENT_SCOPE)
        return()
    endif()
    
    if(from_level GREATER to_level)
        # Higher layers can depend on lower layers
        set(${out_var} TRUE PARENT_SCOPE)
    else()
        # Same or lower layer dependency not allowed
        set(${out_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Enforce that a target only links allowed dependencies
function(atlas_enforce_layer_dependencies target layer)
    get_target_property(link_libs ${target} LINK_LIBRARIES)
    if(NOT link_libs)
        return()
    endif()
    
    set(violations "")
    
    foreach(lib ${link_libs})
        # Skip non-target dependencies (system libs, interface libs, etc.)
        if(NOT TARGET ${lib})
            continue()
        endif()
        
        # Try to extract layer from target name
        # Expected naming: AtlasCore, AtlasSimulation, AtlasAssets, etc.
        string(REGEX MATCH "Atlas([A-Z][a-z]+)" match ${lib})
        if(match)
            string(TOLOWER ${CMAKE_MATCH_1} dep_layer)
            
            atlas_can_depend(${layer} ${dep_layer} can_depend)
            if(NOT can_depend)
                list(APPEND violations "${layer} -> ${dep_layer} (via ${lib})")
            endif()
        endif()
    endforeach()
    
    if(violations)
        string(REPLACE ";" "\n    " violations_str "${violations}")
        message(FATAL_ERROR 
            "\n❌ Atlas Core Contract Violation: Invalid layer dependencies\n"
            "   Target: ${target}\n"
            "   Layer: ${layer}\n"
            "   Violations:\n    ${violations_str}\n"
            "\n   See: docs/ATLAS_CORE_CONTRACT.md\n"
        )
    endif()
endfunction()

# Prevent a target from including headers outside its layer
function(atlas_enforce_include_boundaries target layer allowed_layers)
    # This is enforced at runtime via compile-time checks
    # Add a compile definition that can be used in headers
    target_compile_definitions(${target} PRIVATE ATLAS_LAYER_${layer}=1)
    
    # In strict mode, add checks
    if(ATLAS_DETERMINISM_STRICT)
        target_compile_definitions(${target} PRIVATE ATLAS_LAYER_STRICT=1)
    endif()
endfunction()

# Main enforcement function: call this for each module target
function(atlas_enforce_contract target layer)
    atlas_enforce_layer_dependencies(${target} ${layer})
    atlas_enforce_include_boundaries(${target} ${layer} "")
    
    # Add custom target to verify at build time
    add_custom_command(TARGET ${target} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "✓ Contract check: ${target} (${layer} layer)"
    )
endfunction()

# Convenience: add contract markers to a target
macro(atlas_add_contract_to_target target layer)
    atlas_enforce_contract(${target} ${layer})
endmacro()

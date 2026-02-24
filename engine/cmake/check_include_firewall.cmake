# Include Firewall Check
# Scans simulation source files to ensure they don't include render headers.
#
# Forbidden patterns in sim/ and core/contract/:
#   #include ".*render.*"
#   #include ".*Render.*"
#   #include ".*GLRenderer.*"
#   #include ".*VulkanRenderer.*"
#   #include ".*platform/.*Window.*"

set(VIOLATIONS "")
set(VIOLATION_COUNT 0)

# Directories to scan
set(SCAN_DIRS "")
if(DEFINED SIM_DIR AND EXISTS "${SIM_DIR}")
    list(APPEND SCAN_DIRS "${SIM_DIR}")
endif()
if(DEFINED CONTRACT_DIR AND EXISTS "${CONTRACT_DIR}")
    list(APPEND SCAN_DIRS "${CONTRACT_DIR}")
endif()

foreach(DIR ${SCAN_DIRS})
    file(GLOB_RECURSE SRC_FILES "${DIR}/*.h" "${DIR}/*.cpp")
    foreach(SRC ${SRC_FILES})
        file(READ "${SRC}" CONTENT)
        # Check for render includes
        string(REGEX MATCHALL "#include[^\n]*[Rr]ender[^\n]*" MATCHES "${CONTENT}")
        foreach(MATCH ${MATCHES})
            # Allow RenderAPI.h forward-declared enum usage (used in EngineConfig)
            # but not actual renderer implementation includes
            if(NOT "${MATCH}" MATCHES "RenderAPI")
                math(EXPR VIOLATION_COUNT "${VIOLATION_COUNT} + 1")
                set(VIOLATIONS "${VIOLATIONS}\n  ${SRC}: ${MATCH}")
            endif()
        endforeach()

        # Check for platform window includes
        string(REGEX MATCHALL "#include[^\n]*platform[/\\\\].*Window[^\n]*" MATCHES2 "${CONTENT}")
        foreach(MATCH ${MATCHES2})
            math(EXPR VIOLATION_COUNT "${VIOLATION_COUNT} + 1")
            set(VIOLATIONS "${VIOLATIONS}\n  ${SRC}: ${MATCH}")
        endforeach()

        # Check for GL/Vulkan specific includes
        string(REGEX MATCHALL "#include[^\n]*GL[^\n]*\\.h[^\n]*" GL_MATCHES "${CONTENT}")
        foreach(MATCH ${GL_MATCHES})
            if("${MATCH}" MATCHES "GLRenderer|GL/gl")
                math(EXPR VIOLATION_COUNT "${VIOLATION_COUNT} + 1")
                set(VIOLATIONS "${VIOLATIONS}\n  ${SRC}: ${MATCH}")
            endif()
        endforeach()
    endforeach()
endforeach()

if(VIOLATION_COUNT GREATER 0)
    message(FATAL_ERROR
        "ATLAS INCLUDE FIREWALL VIOLATION\n"
        "Simulation code must not include render/platform headers.\n"
        "Found ${VIOLATION_COUNT} violation(s):${VIOLATIONS}\n"
        "\n"
        "See docs/ATLAS_CORE_CONTRACT.md for rules."
    )
else()
    message(STATUS "Include firewall check passed - no violations found")
endif()

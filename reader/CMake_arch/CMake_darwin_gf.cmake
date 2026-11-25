# macOS (darwin, Apple Silicon) compilation settings - GCC from MacPorts
########################################################################

set (compile "darwin_gf")
set ( lib "dylib")
set ( libp "lib")

# extlib ships boost_filesystem only as Linux/Windows static libraries.
# On macOS the reader uses std::filesystem instead (see
# cfgkernel/HCDI/hcdi_multicfgkernelmgr.cpp), so no boost library is linked.
set ( boost_filesystem_lib "" )

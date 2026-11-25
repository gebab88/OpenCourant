# Compilers for the macOS (Apple Silicon) Gfortran binary - MacPorts gcc15
# Override with OC_FC / OC_CC / OC_CXX to use another GCC installation.
Fortran_comp=${OC_FC:-gfortran-mp-15}
C_comp=${OC_CC:-gcc-mp-15}
CXX_comp=${OC_CXX:-g++-mp-15}
CPP_comp=${OC_CXX:-g++-mp-15}

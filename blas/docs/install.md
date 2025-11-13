# BLAS Module Installation

The BLAS module provides q bindings to BLAS (Basic Linear Algebra Subprograms) libraries, enabling high-performance linear algebra operations directly from q. It is provided as part of the fusionx module.

## Prerequisites

Before using the BLAS module, ensure you have:

1. KDB-X installed 
1. A copy of the fusionx module
1. A BLAS library installed on your system, like one of the following:
   - OpenBLAS 
   - Intel MKL
   - ATLAS
   - BLIS
   - Apple Accelerate (macOS)
   - AMD AOCL
   - Netlib BLAS


## Module Installation

The BLAS module comes bundled with the fusionx module. Explicit installation is not required.

## Loading the Module

From any KDB-X session, you can now import the BLAS library:

```q
q).blas:use(`kx.fusion:blas;([lib:"libopenblas.so"]))
Successfully loaded BLAS library: libopenblas.so
```

## Verifying Installation

Test that the module loaded correctly:

```q
q).blas.dasum[1000#1.0]
1000f

q).blas.ddot[10#1.0; 10#2.0]
20f
```

## Troubleshooting

### Library Not Found
If you receive an error about the library not being found:

1. Verify the BLAS library is installed:
   ```bash
   ldconfig -p | grep blas  # Linux
   ```

2. Check library path is in your system's library search path:
   ```bash
   export LD_LIBRARY_PATH="/path/to/blas/lib:$LD_LIBRARY_PATH"
   ```


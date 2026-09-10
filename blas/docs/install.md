# BLAS Module Installation

The BLAS module provides q bindings to BLAS (Basic Linear Algebra Subprograms) libraries, enabling high-performance linear algebra operations directly from q. It is provided as part of the fusionx module.

## Installing with qmamba

[qmamba](../../README.md#install-module-with-qmamba) is a package manager for KDB-X. It is currently available in a private preview capacity. You are welcome to try it and provide feedback.
```q
qmamba.install `SPECS`CHANNEL!(enlist "q-kx-openblas";("kx";"conda-forge"))
```

!!! note "Blas is a module and fusionx is a module provider"
     The `q-kx-openblas` artefact uses the OpenBLAS library. If you want to install a different BLAS library you can install the fusionx module and specify the BLAS library you want to use

```q
qmamba.install `SPECS`CHANNEL!(("q-kx-fusionx";"mkl");("kx";"conda-forge"))
```

## Installing a Release

### Prerequisites

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

### Module Installation

[Download a release](https://github.com/KxSystems/fusionx/releases) and then unzip to your module directory. The following example assumes the default install location for KDB-X.
```
unzip fusion-l64.zip -d ~/.kx/mod
```

## Loading the Module

From any KDB-X session, you can now import the BLAS module:
```q
q)blas:use(`kx.fusion:blas;([lib:`openblas]))
```
The second parameter to the `use` function specifies the BLAS implementation that you want to use. To use Intel's MKL library you would load the BLAS module like so:
```
q)blas:use(`kx.fusion:blas;([lib:`mkl]))
```
If you are using qmamba, you'll first need to activate the environment you installed the module in.

## Verifying Installation

Test that the module loaded correctly:
```q
q)blas.dasum[1000#1.0]
1000f

q)blas.ddot[10#1.0; 10#2.0]
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


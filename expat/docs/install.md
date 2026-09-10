# Expat Module Installation

## Installing with qmamba

[qmamba](../../README.md#install-module-with-qmamba) is a package manager for KDB-X. It is currently available in a private preview capacity. You are welcome to try it and provide feedback.
```q
qmamba.install `SPECS`CHANNEL!(enlist "q-kx-expat";("kx";"conda-forge"))
```

## Install a Release

### Prerequisites
Before using the Expat module, ensure you have:
1. KDB-X installed 
1. A copy of the fusionx module
1. The Expat library installed on your system

The Expat library is generally pre-installed on macOS and most Linux distribution. If the Expat library is not installed on your system see the installation instructions below.

#### Linux
Use one of the commands below based on your Linux distribution to install the Expat library.
* Debian based
  ```Bash
  apt install libexpat1
  ```
* RHEL based
  ```Bash
  yum install expat
  ```
* Arch based
  ```Bash
  pacman -Sy expat
  ```

To verify that the library is installed:
```bash
ldconfig -p | grep expat
```

#### macOS
Use the [Homebrew](https://brew.sh/) package manager to install the Expat library on macOS.
```Bash
brew install expat
```

#### Windows
Pre-compiled binaries can be downloaded from Expat's GitHub [Releases](https://github.com/libexpat/libexpat/releases) page.

### Module Installation
[Download a release](https://github.com/KxSystems/fusionx/releases) and then unzip to your module directory. The following example assumes the default install location for KDB-X.
```
unzip fusion-l64.zip -d ~/.kx/mod
```

## Loading the Module

From any KDB-X session, you can now import the Expat module:
```q
q)expat:use`kx.fusion:expat
```
If you are using qmamba, you'll first need to activate the environment you installed the module in.

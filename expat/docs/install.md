# Expat Module Installation
The KDB-X Expat module requires the [Expat](https://libexpat.github.io/) library to be
installed on your system. The Expat library is generally pre-installed on macOS and most Linux
distribution. If the Expat library is not installed on your system see the installation instructions
below.

## Prerequisites

Before using the Expat module, ensure you have:

1. KDB-X installed 
1. A copy of the fusionx module
1. An Expat library installed on your system. You can follow the steps below to do this if not already installed.


## Linux
Use one of the commands below based on your Linux distribution to install the Expat library.
### Debian based
```Bash
apt install libexpat1
```
### RHEL based
```Bash
yum install expat
```
### Arch based
```Bash
pacman -Sy expat
```

To verify that the library is installed:
```bash
ldconfig -p | grep expat
```

## macOS
Use the [Homebrew](https://brew.sh/) package manager to install the Expat library on macOS.
```Bash
brew install expat
```

## Windows
Pre-compiled binaries can be downloaded from Expat's GitHub [Releases](https://github.com/libexpat/libexpat/releases) page.

## Module Installation

The Expat module comes bundled with the fusionx module. Explicit installation is not required.

### Loading the Module

From any KDB-X session, you can now import the Expat library:

```q
q)expat:use`kx.fusion:expat
```

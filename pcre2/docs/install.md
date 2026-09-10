# PCRE2 Module Installation

## Installing with qmamba
[qmamba](../../README.md#install-module-with-qmamba) is a package manager for KDB-X. It is currently available in a private preview capacity. You are welcome to try it and provide feedback.
```q
qmamba.install `SPECS`CHANNEL!(enlist "q-kx-pcre2";("kx";"conda-forge"))
```

## Installing a Release

### Prerequisites
Before using the PCRE2 module, ensure you have:
1. KDB-X installed 
1. A copy of the fusionx module
1. The PCRE2 library installed on your system

The PCRE2 library is generally pre-installed on macOS and most Linux distribution. If the PCRE2 library is not installed on your system see the installation instructions
below.

#### Linux
Use one of the commands below based on your Linux distribution to install the PCRE2 library.
* Debian based
  ```Bash
  apt install libpcre2-8-0
  ```
* RHEL based
  ```Bash
  yum install pcre2
  ```
* Arch based
  ```Bash
  pacman -Sy pcre2
  ```

#### macOS
Use the [Homebrew](https://brew.sh/) package manager to install the PCRE2 library on macOS.
```Bash
brew install pcre2
```

### Module Installation
[Download a release](https://github.com/KxSystems/fusionx/releases) and then unzip to your module directory. The following example assumes the default install location for KDB-X.
```
unzip fusion-l64.zip -d ~/.kx/mod
```

## Loading the Module
From any KDB-X session, you can now import the PCRE2 module:
```q
pcre2:use`kx.fusion:pcre2
```
If you are using qmamba, you'll first need to activate the environment you installed the module in.

# PCRE2 Module Installation
The KDB-X PCRE2 module requires the [PCRE2](https://pcre2project.github.io/pcre2/) library to be
installed on your system. The PCRE2 library is generally pre-installed on macOS and most Linux
distribution. If the PCRE2 library is not installed on your system see the installation instructions
below.

## Linux
Use one of the commands below based on your Linux distribution to install the PCRE2 library.
### Debian based
```Bash
apt install libpcre2-8-0
```
### RHEL based
```Bash
yum install pcre2
```
### Arch based
```Bash
pacman -Sy pcre2
```

## macOS
Use the [Homebrew](https://brew.sh/) package manager to install the PCRE2 library on macOS.
```Bash
brew install pcre2
```
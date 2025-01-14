#! /usr/bin/env bash

TAG=$(printf "\x1b[1m\x1b[33m[setup]\x1b[0m")

# install instructions for fxsdk:
# https://git.planet-casio.com/Lephenixnoir/GiteaPC

echo "$TAG updating apt packages"
sudo apt update
sudo apt upgrade -y

# install some build tools (they probably should be installed already, but just in case)
echo "$TAG installing build tools"
sudo apt install -y curl git python3 build-essential pkg-config



# the following is modified from the giteapc install script
# https://git.planet-casio.com/Lephenixnoir/GiteaPC/src/branch/master/install.sh

echo "$TAG installing GiteaPC"

PREFIX="$HOME/.local"

# Download the source code

cd "$(mktemp -d)"
git clone --depth=1 "https://git.planet-casio.com/Lephenixnoir/GiteaPC" giteapc && cd giteapc

# Bootstrap the program by installing it with itself (allowing updates)

python3 giteapc.py install -y Lephenixnoir/GiteaPC

# Check whether the bin folder is already in the PATH

if [[ ! ":$PATH:" =~ ":$PREFIX/bin:" ]]; then

  # Try to find a suitable startup file to extend the PATH in

  profile="$HOME/.profile"
  candidates=".bashrc .zshrc .bash_profile .profile .zprofile"

  for c in $candidates; do
    [[ -f "$HOME/$c" ]] && profile="$HOME/$c"
  done

  echo "$TAG adding $PREFIX/bin to PATH"
  echo "export PATH=\"\$PATH:$PREFIX/bin\"" >> $profile
  source $profile

fi

# install dependencies for stuff we're about to install
echo "$TAG installing dependencies"

# fxsdk
sudo apt install -y cmake python3-pil libusb-1.0-0-dev libsdl2-dev libudisks2-dev libglib2.0-dev libpng-dev
# sh-elf-binutils
sudo apt install -y libmpfr-dev libmpc-dev libppl-dev flex texinfo

# install fxsdk
echo "$TAG installing fxSDK"
giteapc install -y Lephenixnoir/fxsdk

# install cross compiler
echo "$TAG installing cross compiler (this might take a while)"
giteapc install -y  Lephenixnoir/sh-elf-binutils Lephenixnoir/sh-elf-gcc

# install C libraries (we need them to build C++ standard library later)
echo "$TAG installing C libraries"
giteapc install -y Lephenixnoir/OpenLibm Vhex-Kernel-Core/fxlibc

# continue building C++ standard library
echo "$TAG building C++ standard library"
giteapc install -y Lephenixnoir/sh-elf-gcc

# install gint kernel
echo "$TAG installing gint kernel"
giteapc install -y Lephenixnoir/gint Lephenixnoir/libprof

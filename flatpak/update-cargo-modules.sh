#!/bin/sh

# Get URL from user
read -p 'URL: ' url

# example URL https://github.com/etesync/libetebase/archive/v0.5.8/libetebase-0.5.8.tar.gz

# Move to a temporary directory
mkdir temp
pushd temp

# Downloads the tarball
wget -q "${url}"

# Extract it
tar xf *

# Move to source dir
src=$(find . -maxdepth 1 -type d | grep -v -x .)

# Move back to original folder
popd

# Generate module list
python3 ~/Projects/flatpak-builder-tools/cargo/flatpak-cargo-generator.py temp/${src}/Cargo.lock -o libetebase-dependencies-cargo-generator.json

# Cleanup
rm -rf temp


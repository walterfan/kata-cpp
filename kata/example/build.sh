#!/usr/bin/env bash
set -e

# Trust corporate CA (self-signed proxy) for Conan source downloads.
export REQUESTS_CA_BUNDLE=${REQUESTS_CA_BUNDLE:-/etc/ssl/cert.pem}
export SSL_CERT_FILE=${SSL_CERT_FILE:-/etc/ssl/cert.pem}
export CURL_CA_BUNDLE=${CURL_CA_BUNDLE:-/etc/ssl/cert.pem}

conan install . --output-folder=build --build=missing

# Use the Conan-generated preset (cmake >= 3.23).
cmake --preset conan-release
cmake --build --preset conan-release -j4
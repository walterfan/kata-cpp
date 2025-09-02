#!/bin/bash

# Conan Source Finder Script
# Finds absolute paths to source folders for Conan packages

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to find Conan package folder name
find_conan_folder() {
    local package=$1
    local version=$2
    sqlite3 ~/.conan2/p/cache.sqlite3 "SELECT path FROM recipes WHERE reference='$package/$version';" 2>/dev/null
}

# Function to get absolute path to source folder
get_source_path() {
    local package=$1
    local version=$2
    
    local folder=$(find_conan_folder "$package" "$version")
    if [ -n "$folder" ]; then
        echo "$HOME/.conan2/p/$folder/s/src"
    else
        echo ""
    fi
}

# Function to check if source folder exists
check_source_exists() {
    local source_path=$1
    if [ -d "$source_path" ]; then
        echo -e "${GREEN}✓${NC}"
    else
        echo -e "${RED}✗${NC}"
    fi
}

# Function to get package information
get_package_info() {
    local package=$1
    local version=$2
    local folder=$3
    
    local conandata_path="$HOME/.conan2/p/$folder/e/conandata.yml"
    if [ -f "$conandata_path" ]; then
        local url=$(grep -A 1 "url:" "$conandata_path" | tail -1 | sed 's/^[[:space:]]*//')
        local sha256=$(grep "sha256:" "$conandata_path" | sed 's/^[[:space:]]*sha256:[[:space:]]*//')
        echo "$url|$sha256"
    else
        echo "||"
    fi
}

# Function to print package details
print_package_details() {
    local package=$1
    local version=$2
    local source_path=$3
    
    echo -e "${BLUE}Package:${NC} $package/$version"
    echo -e "${BLUE}Source Path:${NC} $source_path"
    
    # Check if source exists
    echo -e "${BLUE}Exists:${NC} $(check_source_exists "$source_path")"
    
    # Get folder name
    local folder=$(find_conan_folder "$package" "$version")
    if [ -n "$folder" ]; then
        echo -e "${BLUE}Folder:${NC} $folder"
        
        # Get package info
        local info=$(get_package_info "$package" "$version" "$folder")
        local url=$(echo "$info" | cut -d'|' -f1)
        local sha256=$(echo "$info" | cut -d'|' -f2)
        
        if [ -n "$url" ] && [ "$url" != "" ]; then
            echo -e "${BLUE}Source URL:${NC} $url"
        fi
        if [ -n "$sha256" ] && [ "$sha256" != "" ]; then
            echo -e "${BLUE}SHA256:${NC} $sha256"
        fi
    fi
    
    echo ""
}

# Function to list files in source directory
list_source_files() {
    local source_path=$1
    local max_files=${2:-10}
    
    if [ -d "$source_path" ]; then
        echo -e "${CYAN}Top $max_files files in source directory:${NC}"
        ls -la "$source_path" | head -$((max_files + 1)) | tail -$max_files
        echo ""
    fi
}

# Main function
main() {
    echo -e "${PURPLE}=== Conan Source Finder ===${NC}"
    echo ""
    
    # Check each package
    echo -e "${BLUE}Checking OpenSSL 1.1.1w...${NC}"
    source_path=$(get_source_path "openssl" "1.1.1w")
    if [ -n "$source_path" ]; then
        print_package_details "openssl" "1.1.1w" "$source_path"
        list_source_files "$source_path" 5
    else
        echo -e "${RED}Package openssl/1.1.1w not found in Conan cache${NC}"
        echo ""
    fi
    
    echo -e "${BLUE}Checking libcurl 8.15.0...${NC}"
    source_path=$(get_source_path "libcurl" "8.15.0")
    if [ -n "$source_path" ]; then
        print_package_details "libcurl" "8.15.0" "$source_path"
        list_source_files "$source_path" 5
    else
        echo -e "${RED}Package libcurl/8.15.0 not found in Conan cache${NC}"
        echo ""
    fi
    
    echo -e "${BLUE}Checking jsoncpp 1.9.6...${NC}"
    source_path=$(get_source_path "jsoncpp" "1.9.6")
    if [ -n "$source_path" ]; then
        print_package_details "jsoncpp" "1.9.6" "$source_path"
        list_source_files "$source_path" 5
    else
        echo -e "${RED}Package jsoncpp/1.9.6 not found in Conan cache${NC}"
        echo ""
    fi
    
    # Summary
    echo -e "${PURPLE}=== Summary ===${NC}"
    echo -e "${BLUE}Conan Cache Directory:${NC} $HOME/.conan2/p/"
    echo -e "${BLUE}Database:${NC} $HOME/.conan2/p/cache.sqlite3"
    echo ""
    
    # Show all OpenSSL versions
    echo -e "${YELLOW}All OpenSSL versions in cache:${NC}"
    sqlite3 ~/.conan2/p/cache.sqlite3 "SELECT reference, path FROM recipes WHERE reference LIKE 'openssl/%';" 2>/dev/null || echo "No OpenSSL packages found"
    echo ""
    
    # Show all libcurl versions
    echo -e "${YELLOW}All libcurl versions in cache:${NC}"
    sqlite3 ~/.conan2/p/cache.sqlite3 "SELECT reference, path FROM recipes WHERE reference LIKE 'libcurl/%';" 2>/dev/null || echo "No libcurl packages found"
    echo ""
}

# Function to show usage
show_usage() {
    echo -e "${PURPLE}Usage:${NC}"
    echo "  $0                    # Show all package details"
    echo "  $0 --quick           # Show only paths"
    echo "  $0 --list-all        # List all packages in cache"
    echo "  $0 --help            # Show this help"
    echo ""
    echo -e "${PURPLE}Examples:${NC}"
    echo "  $0"
    echo "  $0 --quick"
    echo "  $0 --list-all"
}

# Function for quick mode
quick_mode() {
    echo -e "${PURPLE}=== Quick Source Paths ===${NC}"
    echo ""
    
    # Check each package
    source_path=$(get_source_path "openssl" "1.1.1w")
    if [ -n "$source_path" ]; then
        echo -e "${GREEN}openssl/1.1.1w:${NC} $source_path"
    else
        echo -e "${RED}openssl/1.1.1w:${NC} Not found"
    fi
    
    source_path=$(get_source_path "libcurl" "8.15.0")
    if [ -n "$source_path" ]; then
        echo -e "${GREEN}libcurl/8.15.0:${NC} $source_path"
    else
        echo -e "${RED}libcurl/8.15.0:${NC} Not found"
    fi
    
    source_path=$(get_source_path "jsoncpp" "1.9.6")
    if [ -n "$source_path" ]; then
        echo -e "${GREEN}jsoncpp/1.9.6:${NC} $source_path"
    else
        echo -e "${RED}jsoncpp/1.9.6:${NC} Not found"
    fi
}

# Function to list all packages
list_all_packages() {
    echo -e "${PURPLE}=== All Packages in Conan Cache ===${NC}"
    echo ""
    
    sqlite3 ~/.conan2/p/cache.sqlite3 "SELECT reference, path FROM recipes ORDER BY reference;" 2>/dev/null || echo "No packages found"
}

# Parse command line arguments
case "${1:-}" in
    --help|-h)
        show_usage
        exit 0
        ;;
    --quick|-q)
        quick_mode
        exit 0
        ;;
    --list-all|-l)
        list_all_packages
        exit 0
        ;;
    "")
        main
        exit 0
        ;;
    *)
        echo -e "${RED}Unknown option: $1${NC}"
        show_usage
        exit 1
        ;;
esac

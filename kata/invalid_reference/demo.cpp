#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <iomanip>

// Global sequence counter
static int g_sequence_counter = 0;

// Helper function to print messages with auto-increment sequence and line numbers
void print_msg(const char* function_name, int line, const std::string& message) {
    ++g_sequence_counter;
    std::cout << "[" << std::setfill('0') << std::setw(3) << g_sequence_counter 
              << "-" << function_name << "] Line " << line << ": " << message << std::endl;
}


struct secret_item_t {
    std::string name;
    std::string value;
    
    secret_item_t() = default;
    secret_item_t(const std::string& n, const std::string& v) : name(n), value(v) {
        print_msg(__FUNCTION__, __LINE__, "secret_item_t(" + name + ") created at " + std::to_string((uintptr_t)this));
    }
    
    ~secret_item_t() {
        print_msg(__FUNCTION__, __LINE__, "~secret_item_t(" + name + ") destroyed at " + std::to_string((uintptr_t)this));
    }
    
    secret_item_t& operator=(const secret_item_t& other) {
        print_msg(__FUNCTION__, __LINE__, "Assignment: " + std::to_string((uintptr_t)this) + " = " + std::to_string((uintptr_t)&other));
        // This will crash if this == nullptr (your exact error!)
        name = other.name;
        value = other.value;
        return *this;
    }
};

// Simulate your SDK function
bool get_secret_item(const std::string& alias, secret_item_t& item) {
    print_msg(__FUNCTION__, __LINE__, "SDK called with item reference at: " + std::to_string((uintptr_t)&item));
    
    secret_item_t found("secret_" + alias, "value_123");
    item = found;  // ← CRASH POINT if item reference is invalid!
    
    return true;
}

// DANGEROUS PATTERN 1: Reference to local object
secret_item_t& create_local_reference() {
    secret_item_t local_item("local_secret", "local_value");
    print_msg(__FUNCTION__, __LINE__, "Returning reference to local at: " + std::to_string((uintptr_t)&local_item));
    return local_item;  // ❌ DANGER! Object will be destroyed when function ends
}

// DANGEROUS PATTERN 1: Invalid Reference (Reference to destroyed local object)
void demonstrate_invalid_reference(bool should_crash = false) {
    print_msg(__FUNCTION__, __LINE__, "=== Pattern 1: Reference to Local Object ===");
    {
        secret_item_t& ref = create_local_reference();
        print_msg(__FUNCTION__, __LINE__, "Got reference at: " + std::to_string((uintptr_t)&ref));
        print_msg(__FUNCTION__, __LINE__, "Local object destroyed, but reference still exists!");

        // Using this reference is undefined behavior - may crash
        print_msg(__FUNCTION__, __LINE__, "Attempting to use destroyed object reference...");
        if (should_crash) {
            print_msg(__FUNCTION__, __LINE__, "CRASH MODE: Calling get_secret_item with invalid reference!");
            get_secret_item("test1", ref);  // ❌ Undefined behavior - may crash
        } else {
            print_msg(__FUNCTION__, __LINE__, "SAFE MODE: Skipping dangerous reference usage");
        }
    }
}

// DANGEROUS PATTERN 2: Dangling Reference (Your most likely scenario)
void demonstrate_dangling_reference(bool should_crash = false) {
    print_msg(__FUNCTION__, __LINE__, "\n=== CRASH SCENARIO: Dangling Reference ===");
    
    // Step 1: Create a valid object on the heap
    secret_item_t* ptr = new secret_item_t("mysql_password", "initial_value");
    print_msg(__FUNCTION__, __LINE__, "Created valid object at: " + std::to_string((uintptr_t)ptr));
    
    // Step 2: Create a reference to the valid object
    secret_item_t& item_ref = *ptr;
    print_msg(__FUNCTION__, __LINE__, "Created reference to object at: " + std::to_string((uintptr_t)&item_ref));
    
    // Step 3: Use the reference successfully (this works)
    print_msg(__FUNCTION__, __LINE__, "Using reference successfully: " + item_ref.name + " = " + item_ref.value);
 
    // Step 4: Delete the pointer - this frees the memory!
    print_msg(__FUNCTION__, __LINE__, "Deleting the pointer (freeing memory)...");
    void* freed_addr = ptr;  // Save the address for reference
    delete ptr;
    ptr = nullptr;
    print_msg(__FUNCTION__, __LINE__, "Memory at " + std::to_string((uintptr_t)freed_addr) + " is now freed!");
    
    // Step 4.1: Poison the memory to make crash more likely
    if (should_crash) {
        print_msg(__FUNCTION__, __LINE__, "Poisoning freed memory to ensure crash...");
        // Fill the freed memory with garbage to make crash more reliable
        memset(freed_addr, 0xDEADBEEF, sizeof(secret_item_t));
        print_msg(__FUNCTION__, __LINE__, "Using reference failed: " + item_ref.name + " = " + item_ref.value);
 
    }
    
    // Step 5: The reference still "points" to the freed memory location
    print_msg(__FUNCTION__, __LINE__, "Reference still points to freed memory at: " + std::to_string((uintptr_t)&item_ref));
    print_msg(__FUNCTION__, __LINE__, "This is exactly your crash scenario!");
 
    if (should_crash) {
        print_msg(__FUNCTION__, __LINE__, "\nCRASH MODE ENABLED - Using freed memory reference!");
        print_msg(__FUNCTION__, __LINE__, "Calling get_secret_item with dangling reference...");
        
        // This will crash because item_ref points to freed memory
        // The assignment operator will try to access freed memory
        get_secret_item("mysql_password", item_ref);  // ❌ CRASH! Using freed memory
        
        print_msg(__FUNCTION__, __LINE__, "If you see this, the crash didn't happen (memory might still be intact)");
    } else {
        print_msg(__FUNCTION__, __LINE__, "\nSAFE MODE - Crash code is disabled. Use --crash 1 to enable.");
        print_msg(__FUNCTION__, __LINE__, "The reference points to freed memory, but we won't use it.");
    }
}

// CORRECT USAGE: Safe reference handling
void demonstrate_correct_usage() {
    print_msg(__FUNCTION__, __LINE__, "\n=== CORRECT Usage ===");
    secret_item_t valid_item;
    print_msg(__FUNCTION__, __LINE__, "Valid item created at: " + std::to_string((uintptr_t)&valid_item));
    get_secret_item("correct_alias", valid_item);  // ✅ Safe!
    print_msg(__FUNCTION__, __LINE__, "Result: " + valid_item.name + " = " + valid_item.value);
}

// Parse command line arguments
bool parse_crash_argument(int argc, char* argv[]) {
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--crash") == 0) {
            int crash_value = std::atoi(argv[i + 1]);
            return crash_value == 1;
        }
    }
    return false;
}

void print_usage(const char* program_name) {
    print_msg(__FUNCTION__, __LINE__, "Usage: " + std::string(program_name) + " [--crash 0|1]");
    print_msg(__FUNCTION__, __LINE__, "  --crash 0  : Run in safe mode (default)");
    print_msg(__FUNCTION__, __LINE__, "  --crash 1  : Enable crash scenarios (will cause segmentation fault)");
}

int main(int argc, char* argv[]) {
    print_msg(__FUNCTION__, __LINE__, "=== Dangerous Reference Patterns Demo ===");
    
    // Parse command line arguments
    bool should_crash = parse_crash_argument(argc, argv);
    
    if (should_crash) {
        print_msg(__FUNCTION__, __LINE__, "⚠️  CRASH MODE ENABLED - This program WILL crash!");
    } else {
        print_msg(__FUNCTION__, __LINE__, "✅ SAFE MODE - Dangerous code is disabled");
        print_msg(__FUNCTION__, __LINE__, "   Use --crash 1 to enable crash scenarios");
    }

    // Show usage if requested
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }
    
    // Pattern 1: Reference to local object (undefined behavior)
    demonstrate_invalid_reference(should_crash);

    // Pattern 2: Dangling reference (your scenario)
    demonstrate_dangling_reference(should_crash);

    // CORRECT usage (always show this)
    demonstrate_correct_usage();

    print_msg(__FUNCTION__, __LINE__, "=== Demo Complete ===");

    return 0;
}
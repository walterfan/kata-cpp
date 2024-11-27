script_folder="/Users/walter/workspace/walter/code-kata/cpp_kata/build/build/Release/generators"
echo "echo Restoring environment" > "$script_folder/../../../build/Release/generators/deactivate_conanrunenv-release-armv8.sh"
for v in OPENSSL_MODULES
do
    is_defined="true"
    value=$(printenv $v) || is_defined="" || true
    if [ -n "$value" ] || [ -n "$is_defined" ]
    then
        echo export "$v='$value'" >> "$script_folder/../../../build/Release/generators/deactivate_conanrunenv-release-armv8.sh"
    else
        echo unset $v >> "$script_folder/../../../build/Release/generators/deactivate_conanrunenv-release-armv8.sh"
    fi
done


export OPENSSL_MODULES="/Users/walter/.conan2/p/b/opens9d7a8bbc86d89/p/lib/ossl-modules"
-- xmake.lua
set_project("isolated-vm")
set_version("0.0.1")
add_rules("mode.release")

set_languages("gnu++26")
add_defines("EXPORT_IS_EXPORT")

-- 头文件路径
add_includedirs("packages/utility/include")
add_includedirs("/tmp/host-boost-headers")
add_includedirs("packages/third_party/nodejs/deps/nodejs/24.15.0/include/node")
add_includedirs("packages/third_party/v8/deps/nodejs/24.15.0/include/node")
add_includedirs("packages/third_party/v8")
add_includedirs(os.getenv("ANDROID_NDK_HOME") .. "/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/c++/v1")

add_linkdirs("deps/javet")

if is_arch("arm64-v8a") then
    add_cxflags("--target=aarch64-none-linux-android24")
    add_ldflags("--target=aarch64-none-linux-android24")
elseif is_arch("x86_64") then
    add_cxflags("--target=x86_64-none-linux-android24")
    add_ldflags("--target=x86_64-none-linux-android24")
end

add_cxxflags("-fvisibility=hidden", "-fPIC")

-- 排除规则：递归删除 node_modules 和 test 目录下的文件
function exclude_extra()
    remove_files("packages/*/node_modules/**")
    remove_files("packages/*/test/**")
    remove_files("packages/*/tests/**")
end

-- 每个目标添加完文件后调用排除函数
target("utility")
    set_kind("static")
    add_files("packages/utility/**.cc")
    exclude_extra()

target("auto_js")
    set_kind("static")
    add_deps("utility")
    add_files("packages/auto_js/js/**.cc")
    exclude_extra()

target("nodejs")
    set_kind("static")
    add_deps("auto_js")
    add_defines("NAPI_VERSION=10")
    add_files("packages/third_party/nodejs/**.cc")
    exclude_extra()

target("nodejs_v8")
    set_kind("static")
    add_deps("auto_js")
    add_files("packages/third_party/v8/**.cc")
    exclude_extra()

target("napi_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8")
    add_files("packages/auto_js/napi/**.cc")
    exclude_extra()

target("v8_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs_v8")
    add_files("packages/auto_js/v8/**.cc")
    exclude_extra()

target("isolated_vm")
    set_kind("static")
    add_deps("utility", "v8_js", "napi_js")
    add_files("packages/isolated-vm/addon/**.cc")
    add_files("packages/backend_napi_v8/api/**.cc")
    exclude_extra()

target("backend_napi_v8")
    set_kind("shared")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8", "napi_js", "v8_js", "isolated_vm")
    add_files("packages/backend_napi_v8/**.cc")
    exclude_extra()
    add_links("javet-node-android-i18n")
    if is_plat("android") then
        add_links("uv", "android", "log", "EGL", "GLESv2", "OpenSLES")
    end
    set_filename("backend_napi_v8.node")

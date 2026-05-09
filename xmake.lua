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

-- 链接目录
add_linkdirs("deps/javet")

-- 目标三元组
if is_arch("arm64-v8a") then
    add_cxflags("--target=aarch64-none-linux-android24")
    add_ldflags("--target=aarch64-none-linux-android24")
elseif is_arch("x86_64") then
    add_cxflags("--target=x86_64-none-linux-android24")
    add_ldflags("--target=x86_64-none-linux-android24")
end

add_cxxflags("-fvisibility=hidden", "-fPIC")

-- 定义排除规则
local function no_test(file)
    if file:find("test/") or file:find("node_modules/") then
        return false
    end
    return true
end

-- ============= 库定义 =============
target("utility")
    set_kind("static")
    add_files("packages/utility/**.cc", {filter = no_test})

target("auto_js")
    set_kind("static")
    add_deps("utility")
    add_files("packages/auto_js/js/**.cc", {filter = no_test})

target("nodejs")
    set_kind("static")
    add_deps("auto_js")
    add_defines("NAPI_VERSION=10")
    add_files("packages/third_party/nodejs/**.cc", {filter = no_test})

target("nodejs_v8")
    set_kind("static")
    add_deps("auto_js")
    add_files("packages/third_party/v8/**.cc", {filter = no_test})

target("napi_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8")
    add_files("packages/auto_js/napi/**.cc", {filter = no_test})

target("v8_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs_v8")
    add_files("packages/auto_js/v8/**.cc", {filter = no_test})

target("isolated_vm")
    set_kind("static")
    add_deps("utility", "v8_js", "napi_js")
    add_files("packages/isolated-vm/addon/**.cc", {filter = no_test})
    add_files("packages/backend_napi_v8/api/**.cc", {filter = no_test})

target("backend_napi_v8")
    set_kind("shared")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8", "napi_js", "v8_js", "isolated_vm")
    add_files("packages/backend_napi_v8/**.cc", {filter = no_test})
    add_links("javet-node-android-i18n")
    if is_plat("android") then
        add_links("uv", "android", "log", "EGL", "GLESv2", "OpenSLES")
    end
    set_filename("backend_napi_v8.node")

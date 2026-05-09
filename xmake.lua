set_project("isolated-vm")
set_version("0.0.1")
add_rules("mode.release")

-- C++20 模块扫描
set_languages("gnu++26")

-- 全局定义
add_defines("EXPORT_IS_EXPORT")

-- 源码修复中已添加 bits/stdc++.h，此处包含 Boost 和 NDK 头文件路径
add_includedirs("packages/utility/include")
add_includedirs("/tmp/host-boost-headers")
-- Node 和 V8 头文件路径
add_includedirs("packages/third_party/nodejs/deps/nodejs/24.15.0/include/node")
add_includedirs("packages/third_party/v8/deps/nodejs/24.15.0/include/node")
add_includedirs("packages/third_party/v8")
-- bits/stdc++.h 所在目录
add_includedirs(os.getenv("ANDROID_NDK_HOME") .. "/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/c++/v1")

-- 链接目录
add_linkdirs("deps/javet")

-- 根据架构设定目标三元组
if is_arch("arm64-v8a") then
    add_cxflags("--target=aarch64-none-linux-android24")
    add_ldflags("--target=aarch64-none-linux-android24")
elseif is_arch("x86_64") then
    add_cxflags("--target=x86_64-none-linux-android24")
    add_ldflags("--target=x86_64-none-linux-android24")
end

-- 通用编译/链接标志
add_cxxflags("-fvisibility=hidden", "-fPIC")

-- ============ 库定义 ============
target("utility")
    set_kind("static")
    add_files("packages/utility/**.cc")

target("auto_js")
    set_kind("static")
    add_deps("utility")
    add_files("packages/auto_js/js/**.cc")

target("nodejs")
    set_kind("static")
    add_deps("auto_js")
    add_defines("NAPI_VERSION=10")
    add_files("packages/third_party/nodejs/**.cc")

target("nodejs_v8")
    set_kind("static")
    add_deps("auto_js")
    add_files("packages/third_party/v8/**.cc")

target("napi_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8")
    add_files("packages/auto_js/napi/**.cc")

target("v8_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs_v8")
    add_files("packages/auto_js/v8/**.cc")

target("isolated_vm")
    set_kind("static")
    add_deps("utility", "v8_js", "napi_js")
    add_files("packages/isolated-vm/addon/**.cc")
    add_files("packages/backend_napi_v8/api/**.cc")

target("backend_napi_v8")
    set_kind("shared")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8", "napi_js", "v8_js", "isolated_vm")
    add_files("packages/backend_napi_v8/**.cc")
    add_links("javet-node-android-i18n")
    if is_plat("android") then
        add_links("uv", "android", "log", "EGL", "GLESv2", "OpenSLES")
    end
    set_filename("backend_napi_v8.node")

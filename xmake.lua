-- xmake.lua
set_project("isolated-vm")
set_version("0.0.1")
add_rules("mode.release")

-- 全局启用 C++20 模块支持，并强制公开所有模块接口
set_policy("build.c++.modules", true)
set_policy("build.c++.modules.public", true)

set_languages("gnu++26")
add_defines("EXPORT_IS_EXPORT")

local workspace = os.getenv("GITHUB_WORKSPACE") or os.projectdir()

-- Node.js 头文件（/tmp 直接引用）
local node_ver = os.getenv("NODE_HEADERS") or "24.15.0"
local node_dir = path.join("/tmp", "node-v" .. node_ver)

add_includedirs(path.join(node_dir, "src"), {public = true})
add_includedirs(path.join(node_dir, "deps/v8/include"), {public = true})
add_includedirs(path.join(node_dir, "deps/uv/include"), {public = true})
add_includedirs(path.join(node_dir, "include/node"), {public = true})

add_includedirs(path.join(workspace, "packages/utility/include"))
add_includedirs(path.join(workspace, "packages/auto_js/napi/include"))
add_includedirs(path.join(workspace, "packages/backend_napi_v8/runtime"))

add_includedirs("/tmp/host-boost-headers")
add_includedirs(os.getenv("ANDROID_NDK_HOME") .. "/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/c++/v1")

add_cxxflags("-I" .. path.join(node_dir, "src"))
add_cxxflags("-I" .. path.join(node_dir, "deps/v8/include"))
add_cxxflags("-I" .. path.join(node_dir, "deps/uv/include"))
add_cxxflags("-I" .. path.join(node_dir, "include/node"))
add_cxxflags("-I" .. path.join(workspace, "packages/backend_napi_v8/runtime"))

add_linkdirs("deps/javet")

if is_arch("arm64-v8a") then
    add_cxflags("--target=aarch64-none-linux-android24")
    add_ldflags("--target=aarch64-none-linux-android24")
elseif is_arch("x86_64") then
    add_cxflags("--target=x86_64-none-linux-android24")
    add_ldflags("--target=x86_64-none-linux-android24")
end

add_cxxflags("-fvisibility=hidden", "-fPIC")

-- 过滤器
local function filter_files(file)
    if file:find("node_modules") or file:find("/deps/") or file:find("/test/") or file:find("/tests/") or file:find("/benchmark/") then
        return false
    end
    return true
end

target("utility")
    set_kind("static")
    add_files("packages/utility/**.cc", {filter = filter_files})
    -- 公开模块接口，保证依赖方能够找到 util 的模块映射
    set_policy("build.c++.modules.public", true)

target("auto_js")
    set_kind("static")
    add_deps("utility")
    add_files("packages/auto_js/js/**.cc", {filter = filter_files})

target("nodejs")
    set_kind("static")
    add_deps("auto_js")
    add_defines("NAPI_VERSION=10")
    add_files("packages/third_party/nodejs/js_native_api.cc")
    add_files("packages/third_party/nodejs/js_native_api_types.cc")
    add_files("packages/third_party/nodejs/node_api.cc")
    add_files("packages/third_party/nodejs/nodejs.cc")
    add_files("packages/third_party/nodejs/uv.cc")

target("nodejs_v8")
    set_kind("static")
    add_deps("auto_js")
    add_files("packages/third_party/v8/v8.cc")

target("napi_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8")
    add_files("packages/auto_js/napi/**.cc", {filter = filter_files})
    add_includedirs(path.join(workspace, "packages/auto_js/napi/include"), {public = true})

target("v8_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs_v8")
    add_files("packages/auto_js/v8/**.cc", {filter = filter_files})

target("isolated_vm")
    set_kind("static")
    -- 显式依赖 utility，并保证能访问 util 模块接口
    add_deps("utility", "v8_js", "napi_js")
    add_files("packages/isolated-vm/addon/**.cc", {filter = filter_files})
    add_files("packages/backend_napi_v8/api/**.cc", {filter = filter_files})

target("backend_napi_v8")
    set_kind("shared")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8", "napi_js", "v8_js", "isolated_vm")
    add_files("packages/backend_napi_v8/**.cc", {filter = filter_files})
    add_links("javet-node-android-i18n")
    if is_plat("android") then
        add_links("uv", "android", "log", "EGL", "GLESv2", "OpenSLES")
    end
    set_filename("backend_napi_v8.node")

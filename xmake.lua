-- xmake.lua
set_project("isolated-vm")
set_version("0.0.1")
add_rules("mode.release")

-- 基础编译配置
set_languages("gnu++26")
add_defines("EXPORT_IS_EXPORT")
add_cxxflags("-fvisibility=hidden", "-fPIC")

-- 全局查找路径
local ndk_root = os.getenv("ANDROID_NDK_HOME") or "/tmp/android-ndk"
local node_ver = os.getenv("NODE_HEADERS") or "24.15.0"

add_includedirs(
    "packages/utility/include",
    "/tmp/host-boost-headers",  -- Boost 头文件
    path.join(ndk_root, "toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/c++/v1"),
    path.join("packages/third_party/nodejs/deps/nodejs", node_ver, "include/node"),
    path.join("packages/third_party/v8/deps/nodejs", node_ver, "include/node"),
    "packages/third_party/v8"
)

-- 链接目录
add_linkdirs("deps/javet")

-- 各 target 定义（按拓扑顺序）
-- 1. utility 库（底层工具）
target("utility")
    set_kind("static")
    add_files("packages/utility/**.cc")

-- 2. auto_js 核心（依赖 utility）
target("auto_js")
    set_kind("static")
    add_deps("utility")
    add_files("packages/auto_js/js/**.cc")

-- 3. nodejs 导出层（第三方，依赖 auto_js）
target("nodejs")
    set_kind("static")
    add_deps("auto_js")
    add_defines("NAPI_VERSION=10")
    add_files("packages/third_party/nodejs/**.cc")

-- 4. v8 导出层（第三方，依赖 auto_js）
target("nodejs_v8")
    set_kind("static")
    add_deps("auto_js")
    add_files("packages/third_party/v8/**.cc")

-- 5. napi_js 运行时（依赖 nodejs, nodejs_v8, auto_js, utility）
target("napi_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8")
    add_files("packages/auto_js/napi/**.cc")

-- 6. v8_js 运行时（依赖 nodejs_v8, auto_js, utility）
target("v8_js")
    set_kind("static")
    add_deps("utility", "auto_js", "nodejs_v8")
    add_files("packages/auto_js/v8/**.cc")

-- 7. isolated-vm addon（依赖 v8_js, napi_js, utility）
target("isolated_vm")
    set_kind("static")
    add_deps("utility", "v8_js", "napi_js")
    add_files("packages/isolated-vm/addon/**.cc")
    add_files("packages/backend_napi_v8/api/**.cc")

-- 8. 最终 .node 插件
target("backend_napi_v8")
    set_kind("shared")
    add_deps("utility", "auto_js", "nodejs", "nodejs_v8", "napi_js", "v8_js", "isolated_vm")
    add_files("packages/backend_napi_v8/**.cc")
    -- 链接 javet SO
    add_links("javet-node-android-i18n")
    -- 平台链接库
    if is_plat("android") then
        add_links("uv", "android", "log", "EGL", "GLESv2", "OpenSLES")
    end
    set_filename("backend_napi_v8.node")

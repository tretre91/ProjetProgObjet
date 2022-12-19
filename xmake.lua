add_rules("mode.release", "mode.debug")
set_allowedplats("windows", "macosx", "linux")

add_requires("fltk 1.4.0")

-- Platform specific rules

rule("windows")
    on_load(function (target)
        target:add("files", "./lib/OpenGL-windows.o")
        -- add_links("./lib/fltk-1.4-windows/*.a")
    end)

rule("macos")
    on_load(function (target)
        target:add("files", "./lib/OpenGL-macosx.o")
        target:add("cxxflags", "-Wno-invalid-source-encoding", "-Wno-deprecated-declarations")
        -- add_links("./lib/fltk-1.4-macosx/*.a")
    end)

rule("ubuntu")
    on_load(function (target)
        target:add("files", "./lib/OpenGL-ubuntu.o")
        target:add_defines("UBUNTU")
        -- add_links("./lib/fltk-1.4-ubuntu/*.a")
    end)

rule("linux")
    on_load(function (target)
        target:add("files", "./lib/OpenGL-linux.o")
        -- target:add("links", "./lib/fltk-1.4-linux/*.a")
        target:add("includedirs", "lib/fmod/4.3")
        target:add("linkdirs", "lib/fmod/4.3")
        target:add("rpathdirs", "lib/fmod/4.3")
        if is_arch("x86") then
            target:add("links", "fmodex")
        else
            target:add("links", "fmodex64")
        end
    end)

rule("custom_config")
    if is_os("windows") then
        add_deps("windows")
    elseif is_os("macosx") then 
        add_deps("macosx")
    else -- linux
        if linuxos.name() == "ubuntu" then
            add_deps("ubuntu")
        else
            add_deps("linux")
        end
        on_load(function (target)
            target:add("syslinks", "GLU", "GL", "X11", "Xfixes", "dl", "m", "Xext", "Xft", "Xrender")
        end)
    end

-- target

target("labh")
    set_kind("binary")
    add_rules("custom_config")
    add_cxxflags("-Wall")
    add_packages("fltk")
    add_includedirs("include", "include/immutable", "lib/fmt-9.0.0/include")
    add_defines("FMT_HEADER_ONLY")
    add_files("./src/*.cc", "./src/*.cpp")

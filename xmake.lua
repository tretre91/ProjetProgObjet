add_rules("mode.release", "mode.debug")
set_allowedplats("windows", "macosx", "linux")
set_languages("c++17")

-- Platform specific rules

rule("windows")
    on_load(function (target)
        target:add("files", "./lib/OpenGL-windows.o")
        target:add("linkdirs", "lib/fltk-1.4-windows")
    end)

rule("macos")
    on_load(function (target)
        target:add("files", "./lib/OpenGL-macosx.o")
        target:add("cxxflags", "-Wno-invalid-source-encoding", "-Wno-deprecated-declarations")
        target:add("linkdirs", "lib/fltk-1.4-macosx")
    end)

rule("ubuntu")
    on_load(function (target)
        target:add("files", "./lib/OpenGL-ubuntu.o")
        target:add("linkdirs", "lib/fltk-1.4-ubuntu")
        target:add_defines("UBUNTU")
    end)

rule("linux")
    on_load(function (target)
        target:add("files", "./lib/OpenGL-linux.o")
        target:add("includedirs", "lib/fmod/4.3")
        target:add("linkdirs", "lib/fmod/4.3", "lib/fltk-1.4-linux")
        target:add("rpathdirs", "$(projectdir)/lib/fmod/4.3")
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
            target:add("syslinks", "GLU", "GL", "X11", "Xfixes", "dl", "m", "Xext", "Xft", "Xrender", "fontconfig")
        end)
    end

-- target

target("labh")
    set_kind("binary")
    add_rules("custom_config")
    add_cxxflags("-Wall")
    add_links("fltk_jpeg", "fltk_png", "fltk_z", "fltk_gl", "fltk")
    add_includedirs("include", "include/immutable", "lib/fmt-9.0.0/include")
    add_defines("FMT_HEADER_ONLY")
    add_files("./src/*.cc")

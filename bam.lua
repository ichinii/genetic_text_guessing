src_dir = "."
obj_dir = "obj"

AddTool(function(s)
	s.cc.flags:Add("-Wall")
	s.cc.flags:Add("-Wextra")
	s.cc.flags_cxx:Add("--std=c++17")
	s.cc.includes:Add(src_dir)

	s.cc.Output = function(s, input)
		input = input:gsub("^"..src_dir.."/", "")
		return PathJoin(obj_dir, PathBase(input))
	end
end)

s = NewSettings()

src = CollectRecursive(PathJoin(src_dir, "*.cpp"))
obj = Compile(s, src)
bin = Link(s, "bin", obj)
PseudoTarget("compile", bin)
PseudoTarget("c", bin)

AddJob("run", "running '"..bin.."'...", "./"..bin)
AddDependency("run", bin)
DefaultTarget(bin)

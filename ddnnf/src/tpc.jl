using Distributions
using DataStructures

include("var.jl")
include("ddnnf.jl")
include("mc.jl")
include("pc.jl")
include("dac.jl")

pdac = pdac_from_file(ARGS[1])
pc = get_pc(pdac.pnnf, 1)
println(ARGS[1], ", ", pc)

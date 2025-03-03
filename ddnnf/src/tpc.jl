using Distributions
using DataStructures

include("var.jl")
include("ddnnf.jl")
include("mc.jl")
include("pc.jl")
include("dac.jl")

# pdac = pdac_from_file(ARGS[1])
pnnf = ddnnf_from_file(ARGS[1] * ".pnnf", true, vp)
apnnf = annotate_pc(pnnf)
pc = get_pc(apnnf, 1)
println(ARGS[1], ", ", pc)

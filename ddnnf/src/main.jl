using Profile
using BenchmarkTools
using CairoMakie
using DataFrames
using CSV
using Distributions
using DataStructures
# using OhMyThreads

include("var.jl")
include("ddnnf.jl")
include("mc.jl")
include("pc.jl")
include("dac.jl")

# dac = dac_from_file(ARGS[1])
pdac = pdac_from_file(ARGS[1])

# println(get_mc(dac.pnnf, 1))
println(get_pc(pdac.pnnf, 1))
println(get_pc(pdac.pnnf, 1) / 10^6)

Y, Yl, Yh = appmc(pdac, 1)
e, el, eh = appmc_l(pdac, 1)
# @profile Y, Yl, Yh = appmc(pdac, 100)
# Profile.print()

@time Y, Yl, Yh = appmc(pdac, 1000)
@time e, el, eh = appmc_l(pdac, 1000)

using BenchmarkTools
using CairoMakie
using DataFrames
using CSV
using Distributions
using DataStructures

include("var.jl")
include("ddnnf.jl")
include("mc.jl")
include("pc.jl")
include("dac.jl")

pdac = pdac_from_file(ARGS[1])

Y, Yl, Yh = appmc(pdac, 10000)
X = 1 : length(Y)

fc(x) = x.file == ARGS[1]
# data = CSV.read("/home/users/ozeyen/ddnnf/mc.csv", DataFrame, types = Dict("mc" => BigInt))
data = CSV.read("mc.csv", DataFrame, types = Dict("mc" => BigInt))
data = filter(fc, data)
TMC = -1
if length(data.mc) == 1
    global TMC = data.mc[1]
end

f = Figure(size = (1000, 1000))
a1 = Axis(f[1, 1])

scatterlines!(a1, X, Y, marker = :cross, markersize = 5)
scatterlines!(a1, X, Yl, marker = :cross, markersize = 5)
scatterlines!(a1, X, Yh, marker = :cross, markersize = 5)

if TMC != -1
    scatterlines!(a1, [X[begin], X[end]], [TMC, TMC])
    println("tmc ", TMC)
end
save(ARGS[1] * ".clt.png", f)

println("pc ", get_pc(pdac.pnnf, 1))
println("s ", Y[end])
println("sl ", Yl[end])
println("sh ", Yh[end])

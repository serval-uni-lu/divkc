using BenchmarkTools
using CairoMakie
using DataFrames
using CSV
using Distributions

include("var.jl")
include("ddnnf.jl")
include("mc.jl")
include("dac.jl")

dac = dac_from_file(ARGS[1])
fc(x) = x.file == ARGS[1]
# data = CSV.read("/home/users/ozeyen/ddnnf/mc.csv", DataFrame, types = Dict("mc" => BigInt))
data = CSV.read("mc.csv", DataFrame, types = Dict("mc" => BigInt))
data = filter(fc, data)
TMC = -1
if length(data.mc) == 1
    global TMC = data.mc[1]
end

println("sp ", get_mc(dac.pnnf, 1))
println("su ", get_mc(dac.unnf, 1))
println("sl ", applb(dac, 500, 300, 0.01))

Y, Yl, Yh = appmc(dac, 10000)
X = 1:length(Y)

f = Figure(size = (2000, 1000))
a1 = Axis(f[1, 1])
a2 = Axis(f[1, 2])
scatterlines!(a1, X, Y, marker = :cross, markersize = 5)
scatterlines!(a1, X, Yl, marker = :cross, markersize = 5)
scatterlines!(a1, X, Yh, marker = :cross, markersize = 5)

if TMC != -1
    scatterlines!(a1, [X[begin], X[end]], [TMC, TMC])

    scatterlines!(a2, X, Y ./ TMC, marker = :cross, markersize = 5)
    scatterlines!(a2, X, Yl ./ TMC, marker = :cross, markersize = 5)
    scatterlines!(a2, X, Yh ./ TMC, marker = :cross, markersize = 5)
end
save(ARGS[1] * ".png", f)

println("s ", Y[end])

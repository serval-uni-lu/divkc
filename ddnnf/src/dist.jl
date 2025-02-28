using Distributed

@everywhere begin
    using Distributed
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
end

# dac = dac_from_file(ARGS[1])
pdac = pdac_from_file(ARGS[1])

# println(get_mc(dac.pnnf, 1))
println(get_pc(pdac.pnnf, 1))
println(get_pc(pdac.pnnf, 1) / 10^6)

function lemc(dac :: PDAC)
    mc = get_pc(dac.pnnf, 1)
    sigma = BigInt(0)

    # sigma = sum(pmap(BigInt(1):mc) do i
    #     s = get_path(dac.pnnf, i)
    #     lunnf = annotate_mc(dac.unnf, s)
    #     ai = get_mc(lunnf, 1)
    #     ai
    # end)
    
    sigma = @distributed (+) for i in BigInt(1):mc
        s = get_path(dac.pnnf, i)
        lunnf = annotate_mc(dac.unnf, s)
        ai = get_mc(lunnf, 1)
        GC.gc()
        ai
    end

    return sigma
end

println("es ", lemc(pdac))

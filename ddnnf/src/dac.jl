struct DAC
    pvar :: Set{Var}
    pnnf :: ADDNNF
    unnf :: ADDNNF
end

function dac_from_file(path :: String)
    vp = Set{Var}()

    # parse log file
    for line in eachline(path * ".proj.log")
        if startswith(line, "c p show ")
            tmp = map(strip, split(line[length("c p show "):end]))

            toint(x) = Base.parse(Int64, x)
            nonzero(x) = x != 0

            y = map(mkVar, filter(nonzero, map(toint, tmp)))
            union!(vp, y)
        end
    end

    # proj ddnnf
    # TODO
    # potential bug in use of projection set
    pnnf = ddnnf_from_file(path * ".pnnf", true, vp)
    # pnnf = ddnnf_from_file(path * ".pnnf")
    apnnf = annotate_mc(pnnf)

    # upper bound ddnnf
    unnf = ddnnf_from_file(path * ".unnf")
    aunnf = annotate_mc(unnf)

    return DAC(vp, apnnf, aunnf)
end

function appmc(dac :: DAC, N :: Int64)
    Y = Vector{BigFloat}()
    vr = Vector{BigInt}()
    X = Vector{Float64}()

    smc = Dict{Set{Lit}, BigInt}()

    keep(x) = mkVar(x) in dac.pvar

    mc = get_mc(dac.pnnf, 1)
    b = time()
    sigma = BigInt(0)
    n = 0

    lck = ReentrantLock()

    Threads.@threads for i in 1:N
        s = filter(keep, Set(sample(dac.pnnf)))
        lunnf = annotate_mc(dac.unnf.nnf, s)
        ai = get_mc(lunnf, 1)

        lock(lck) do
            smc[s] = ai
            n += 1
            sigma += ai

            push!(Y, mc * sigma / n)
            push!(X, time() - b)
            push!(vr, ai)
        end
    end
    smc = collect(values(smc))
    sort!(smc)

    # println("s ", get_mc(dac.pnnf, 1) * sigma / N)
    return (mc * sigma / n, vr, X, Y, smc)
end

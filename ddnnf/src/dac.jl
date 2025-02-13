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

function cleanup(smc :: Dict{BigInt, BigInt}, k :: Int64)
    f(x) = x[2]
    y = collect(smc)
    sort!(y, by = f)

    tl = y[k][2]
    th = y[end-k][2]

    p(x) = x[2] <= tl || x[2] >= th
    filter!(p, smc)
end

function map_model(m :: Set{Lit})
    m = collect(m)
    sort!(m, by = toIndex)

    res = BigInt(0)
    for i in m
        res <<= 1
        if sign(i) == 1
            res += 1
        end
    end
    return res
end

function appmc(dac :: DAC, N :: Int64, k :: Int64)
    Y = Vector{BigFloat}()
    vr = Vector{BigInt}()
    X = Vector{Float64}()

    smc = Dict{BigInt, BigInt}()

    keep(x) = mkVar(x) in dac.pvar

    mc = get_mc(dac.pnnf, 1)
    b = time()
    sigma = BigInt(0)
    n = 0

    lck = ReentrantLock()

    Threads.@threads for i in 1:N
        s = Set(sample(dac.pnnf))
        lunnf = annotate_mc(dac.unnf.nnf, s)
        ai = get_mc(lunnf, 1)

        lock(lck) do
            n += 1
            sigma += ai

            push!(Y, mc * sigma / n)
            push!(X, time() - b)
            push!(vr, ai)

            smc[map_model(s)] = ai

            if length(smc) >= 3 * k
                cleanup(smc, k)
            end
        end
    end
    smc = collect(values(smc))
    sort!(smc)

    # println("s ", get_mc(dac.pnnf, 1) * sigma / N)
    return (mc, mc * sigma / n, vr, X, Y, smc)
end

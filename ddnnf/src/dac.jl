struct DAC
    pvar :: Set{Var}
    pnnf :: ADDNNF
    unnf :: ADDNNF
end

struct PDAC
    pvar :: Set{Var}
    pnnf :: PCDDNNF
    unnf :: DDNNF
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

function pdac_from_file(path :: String)
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
    apnnf = annotate_pc(pnnf)

    # upper bound ddnnf
    unnf = ddnnf_from_file(path * ".unnf")
    # aunnf = annotate_mc(unnf)

    return PDAC(vp, apnnf, unnf)
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

function appmc(dac :: DAC, N :: Int64)
    Y = Vector{BigFloat}()
    L = Vector{BigInt}()
    # Yl = Vector{BigFloat}()
    # Yh = Vector{BigFloat}()
    S = Vector{BigFloat}()

    total = BigInt(0)
    z = quantile(Normal(), 1 - 0.05)

    lck = ReentrantLock()

    Threads.@threads for i in 1:N
    # Threads.@threads for id in rand(BigInt(1) : get_mc(dac.pnnf, 1), N)
        s = Set(sample(dac.pnnf))
        # s = Set(get_solution(dac.pnnf, id))
        lunnf = annotate_mc(dac.unnf.nnf, s)
        ai = get_mc(lunnf, 1) * get_mc(dac.pnnf, 1)


        lock(lck) do
            li = length(Y) + 1
            total += ai
            m = total / li
            push!(L, ai)

            var = sum((L .- m) .^ 2) / (li - 1)
            sd = sqrt(var) / sqrt(li)

            push!(Y, m)
            # push!(Yl, m - z * sd)
            # push!(Yh, m + z * sd)
            push!(S, sd)
        end
    end

    # return Y, Yl, Yh
    return Y, Y .- z .* S, Y .+ z .* S
end

function appmc(dac :: PDAC, N :: Int64)
    Y = Vector{BigFloat}()
    L = Vector{BigInt}()
    # Yl = Vector{BigFloat}()
    # Yh = Vector{BigFloat}()
    S = Vector{BigFloat}()

    total = BigInt(0)
    z = quantile(Normal(), 1 - 0.01)

    lck = ReentrantLock()

    Threads.@threads for i in 1:N
    # Threads.@threads for id in rand(BigInt(1) : get_mc(dac.pnnf, 1), N)
        s = get_path(dac.pnnf, rand(BigInt(1) : get_pc(dac.pnnf, 1)))
        # s = Set(get_solution(dac.pnnf, id))
        lunnf = annotate_mc(dac.unnf, s)
        ai = get_mc(lunnf, 1) * get_pc(dac.pnnf, 1)


        lock(lck) do
            li = length(Y) + 1
            total += ai
            m = total / li
            push!(L, ai)

            var = sum((L .- m) .^ 2) / (li - 1)
            sd = sqrt(var) / sqrt(li)

            push!(Y, m)
            # push!(Yl, m - z * sd)
            # push!(Yh, m + z * sd)
            push!(S, sd)
        end
    end

    # return Y, Yl, Yh
    return Y, Y .- z .* S, Y .+ z .* S
end

function emc(dac :: DAC)
    mc = get_mc(dac.pnnf, 1)
    lck = ReentrantLock()
    sigma = BigInt(0)

    Threads.@threads for i in BigInt(1):mc
        s = Set(get_solution(dac.pnnf, i))
        lunnf = annotate_mc(dac.unnf.nnf, s)
        ai = get_mc(lunnf, 1)

        lock(lck) do
            sigma += ai
        end
    end

    return sigma
end

function emc(dac :: PDAC)
    mc = get_pc(dac.pnnf, 1)
    lck = ReentrantLock()
    sigma = BigInt(0)

    Threads.@threads for i in BigInt(1):mc
        s = get_path(dac.pnnf, i)
        lunnf = annotate_mc(dac.unnf, s)
        ai = get_mc(lunnf, 1)

        lock(lck) do
            sigma += ai
        end
    end

    return sigma
end

function demc(dac :: PDAC)
    mc = get_pc(dac.pnnf, 1)
    lck = ReentrantLock()
    sigma = BigInt(0)

    Threads.@threads for i in BigInt(1):BigInt(1000):mc
        lsigma = BigInt(0)

        for id in i:min(i + 1000 - 1, mc)
            s = get_path(dac.pnnf, id)
            lunnf = annotate_mc(dac.unnf, s)
            ai = get_mc(lunnf, 1)
            lsigma += ai
        end

        lock(lck) do
            sigma += lsigma
        end
    end

    return sigma
end

function appmc2(dac :: DAC, N :: Int64, k :: Int64)
    mc = get_mc(dac.pnnf, 1)
    N = min(mc, BigInt(N))

    lck = ReentrantLock()

    eps = Vector{BigInt}()
    
    Threads.@threads for i in 1:k
        m = Set{BigInt}()
        while length(m) < N
            push!(m, rand(BigInt(1):mc))
        end

        estimate = BigInt(1)
        for id in collect(m)
            s = Set(get_solution(dac.pnnf, id))
            lunnf = annotate_mc(dac.unnf.nnf, s)
            ai = get_mc(lunnf, 1)

            estimate += ai
        end

        lock(lck) do
            push!(eps, estimate)
        end
    end

    mie = minimum(eps)
    mae = maximum(eps)
    return mc * mie / N, mc * mae / N, (N ./ eps) ./ mc
end

function applb(dac :: DAC, N :: Int64, k :: Int64, a :: Float64)
    mc = BigFloat(get_mc(dac.pnnf, 1) * get_mc(dac.unnf, 1))
    lck = ReentrantLock()

    k = min(BigInt(k), get_mc(dac.pnnf, 1))

    for i in 1:N
        m = Set{BigInt}()
        while length(m) < k
            push!(m, rand(BigInt(1):get_mc(dac.pnnf, 1)))
        end

        lmc = BigInt(0)
        for id in collect(m)
            s = Set(get_solution(dac.pnnf, id))
            lunnf = annotate_mc(dac.unnf.nnf, s)
            ai = get_mc(lunnf, 1)
            lmc += ai
        end

        lmc = get_mc(dac.pnnf, 1) * lmc / (k * 2^a)
        lock(lck) do
            mc = min(mc, lmc)
        end
    end

    return mc
end

function eps(dac :: DAC, N :: Int64, k :: Int64)
    low = PriorityQueue{BigInt, BigInt}(Base.Order.ReverseOrdering())
    high = PriorityQueue{BigInt, BigInt}()

    v1 = Vector{BigInt}()
    v2 = Vector{BigInt}()

    lck = ReentrantLock()

    Threads.@threads for i in 1:N
        id = rand(BigInt(1) : get_mc(dac.pnnf, 1))
        s = Set(get_solution(dac.pnnf, id))
        lunnf = annotate_mc(dac.unnf.nnf, s)
        ai = get_mc(lunnf, 1)

        lock(lck) do
            low[id] = ai
            high[id] = ai

            if length(low) > k
                dequeue!(low)
                push!(v1, sum(values(low)))
            end

            if length(high) > k
                dequeue!(high)
                push!(v2, sum(values(high)))
            end
        end
    end

    return v1, v2, sum(values(low)), sum(values(high))
end

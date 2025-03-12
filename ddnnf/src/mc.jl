struct ADDNNF
    nnf :: DDNNF
    assumps :: Set{Lit}
    mc :: Vector{BigInt}
end

function get_mc(nnf :: ADDNNF, e :: Edge)
    for l in get_literals(nnf.nnf, e)
        if (~l) in nnf.assumps
            return 0
        end
    end

    nfree = e.e_free - e.b_free + 1
    for v in get_free_vars(nnf.nnf, e)
        l = mkLit(v, false)
        if (l in nnf.assumps) || ((~l) in nnf.assumps)
            nfree -= 1
        end
    end

    # f(x) = mkLit(x, false) in nnf.assumps || mkLit(x, true) in nnf.assumps
    # nfree = e.e_free - e.b_free + 1 - sum(f, get_free_vars(nnf.nnf, e); init = 0)
    # return get_mc(nnf, e.child) * BigInt(2)^(nfree)
    return get_mc(nnf, e.child) * (BigInt(2) << (nfree))
end

get_mc(nnf :: ADDNNF, i :: Int64) = nnf.mc[i]

function annotate_mc(nnf :: DDNNF, assumps :: Set{Lit} = Set{Lit}())
    res = ADDNNF(nnf, assumps, Vector{BigInt}(undef, length(nnf.nodes)))

    for i in nnf.ordering
        annotate_mc(res, i, nnf.nodes[i])
    end

    return res
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: FalseNode)
    nnf.mc[i] = 0
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: TrueNode)
    nnf.mc[i] = 1
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: UnaryNode)
    nnf.mc[i] = get_mc(nnf, n.child)
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: OrNode)
    f(x) = get_mc(nnf, x)
    nnf.mc[i] = sum(f, n.children)
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: AndNode)
    f(x) = get_mc(nnf, x)
    nnf.mc[i] = prod(f, n.children)
end



function sample(nnf :: ADDNNF, s :: Vector{Lit}, i :: Int64, n :: TrueNode)
    return []
end

function sample(nnf :: ADDNNF, s :: Vector{Lit}, i :: Int64, n :: UnaryNode)
    union!(s, get_literals(nnf.nnf, n.child))
    for v in get_free_vars(nnf.nnf, n.child)
        push!(s, mkLit(v, rand((true, false))))
    end
    return [n.child.child]
end

function sample(nnf :: ADDNNF, s :: Vector{Lit}, i :: Int64, n :: OrNode)
    x = rand(BigInt(1):get_mc(nnf, i))
    for c in n.children
        cmc = get_mc(nnf, c)

        if x <= cmc
            union!(s, get_literals(nnf.nnf, c))
            for v in get_free_vars(nnf.nnf, c)
                push!(s, mkLit(v, rand((true, false))))
            end
            return [c.child]
        else
            x -= cmc
        end
    end
end

function sample(nnf :: ADDNNF, s :: Vector{Lit}, i :: Int64, n :: AndNode)
    f(c) = c.child
    return map(f, n.children)
end

# only works if annotate_mc has been called with assumps an empty set
function sample(nnf :: ADDNNF)
    stack = [Int64(1)]
    res = Vector{Lit}()

    while length(stack) > 0
        x = pop!(stack)
        y = sample(nnf, res, x, nnf.nnf.nodes[x])
        for i in y
            push!(stack, i)
        end
    end

    return res
end

function get_solution(nnf :: ADDNNF, s :: Vector{Lit}, id :: BigInt, i :: Int64, n :: TrueNode)
    return []
end

function get_solution(nnf :: ADDNNF, s :: Vector{Lit}, id :: BigInt, i :: Int64, n :: FalseNode)
    return []
end

function get_solution(nnf :: ADDNNF, s :: Vector{Lit}, id :: BigInt, i :: Int64, n :: UnaryNode)
    union!(s, get_literals(nnf.nnf, n.child))
    mc = get_mc(nnf, i)
    for v in get_free_vars(nnf.nnf, n.child)
        l = mkLit(v, false)
        if (l in nnf.assumps)
            push!(s, l)
        elseif ((~l) in nnf.assumps)
            push!(s, ~l)
        elseif id <= (mc / 2)
            push!(s, l)
            mc /= 2
        else
            mc /= 2
            id -= mc
            push!(s, ~l)
        end
    end
    if id > get_mc(nnf, n.child.child)
        println("u ", id, " >= ", get_mc(nnf, n.child.child))
    end
    return [(id, n.child.child)]
end

function get_solution(nnf :: ADDNNF, s :: Vector{Lit}, id :: BigInt, i :: Int64, n :: OrNode)
    if id > get_mc(nnf, i)
        println("o ", id, " <= ", get_mc(nnf, i))
    end
    for c in n.children
        cmc = get_mc(nnf, c)

        if id <= cmc
            union!(s, get_literals(nnf.nnf, c))

            for v in get_free_vars(nnf.nnf, c)
                l = mkLit(v, false)
                if (l in nnf.assumps)
                    push!(s, l)
                elseif ((~l) in nnf.assumps)
                    push!(s, ~l)
                elseif id <= (cmc / 2)
                    push!(s, l)
                    cmc /= 2
                else
                    cmc /= 2
                    id -= cmc
                    push!(s, ~l)
                end
            end

            if id > get_mc(nnf, c.child)
                println("- o ", id, " >= ", get_mc(nnf, c.child))
            end

            return [(id, c.child)]
        else
            id -= cmc
        end
    end

    # return [(id, n.child.child)]
end

function get_solution(nnf :: ADDNNF, s :: Vector{Lit}, id :: BigInt, i :: Int64, n :: AndNode)
    res = Vector{Tuple{BigInt, Int64}}()

    tmc = get_mc(nnf, i)
    if id > tmc
        println("a ", id, " <= ", tmc)
    end
    for c in n.children
        cmc = get_mc(nnf, c)

        x = ((id - 1) % cmc) + 1
        y = div(id - 1, cmc) + 1

        if x > cmc
            println("- a ", id, " <= ", tmc)
        end

        push!(res, (x, c.child))
        id = y
    end

    return res
end

function get_solution(nnf :: ADDNNF, id :: BigInt)
    stack = [(id, Int64(1))]
    res = Vector{Lit}()

    while length(stack) > 0
        lid, x = pop!(stack)
        y = get_solution(nnf, res, lid, x, nnf.nnf.nodes[x])
        for i in y
            push!(stack, i)
        end
    end

    return res
end

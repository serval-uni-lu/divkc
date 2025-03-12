function get_mc(nnf :: DDNNF, e :: Edge)
    for l in get_literals(nnf, e)
        if (~l) in nnf.assumps
            return 0
        end
    end

    nfree = e.e_free - e.b_free + 1
    for v in get_free_vars(nnf, e)
        l = mkLit(v, false)
        if (l in nnf.assumps) || ((~l) in nnf.assumps)
            nfree -= 1
        end
    end

    # f(x) = mkLit(x, false) in nnf.assumps || mkLit(x, true) in nnf.assumps
    # nfree = e.e_free - e.b_free + 1 - sum(f, get_free_vars(nnf.nnf, e); init = 0)
    # return get_mc(nnf, e.child) * BigInt(2)^(nfree)
    return get_mc(nnf, nnf.nodes[e.child]) * (BigInt(1) << (nfree))
end

get_mc(nnf :: DDNNF, i :: Int64) = get_mc(nnf, nnf.nodes[i])
get_mc(nnf :: DDNNF, n :: UnaryNode) = n.mc
get_mc(nnf :: DDNNF, n :: OrNode) = n.mc
get_mc(nnf :: DDNNF, n :: AndNode) = n.mc
get_mc(nnf :: DDNNF, n :: TrueNode) = BigInt(1)
get_mc(nnf :: DDNNF, n :: FalseNode) = BigInt(0)

function annotate_mc(nnf :: DDNNF, assumps :: Set{Lit} = Set{Lit}())
    nnf.assumps = assumps

    for i in nnf.ordering
        annotate_mc(nnf, nnf.nodes[i])
    end
end

function annotate_mc(nnf :: DDNNF, n :: FalseNode)
end

function annotate_mc(nnf :: DDNNF, n :: TrueNode)
end

function annotate_mc(nnf :: DDNNF, n :: UnaryNode)
    n.mc = get_mc(nnf, n.child)
end

function annotate_mc(nnf :: DDNNF, n :: OrNode)
    f(x) = get_mc(nnf, x)
    n.mc = sum(f, n.children)
end

function annotate_mc(nnf :: DDNNF, n :: AndNode)
    f(x) = get_mc(nnf, x)
    n.mc = prod(f, n.children)
end



function sample(nnf :: DDNNF, s :: Vector{Lit}, n :: TrueNode)
    return []
end

function sample(nnf :: DDNNF, s :: Vector{Lit}, n :: UnaryNode)
    union!(s, get_literals(nnf, n.child))
    for v in get_free_vars(nnf, n.child)
        push!(s, mkLit(v, rand((true, false))))
    end
    return [n.child.child]
end

function sample(nnf :: DDNNF, s :: Vector{Lit}, n :: OrNode)
    x = rand(BigInt(1):get_mc(nnf, i))
    for c in n.children
        cmc = get_mc(nnf, c)

        if x <= cmc
            union!(s, get_literals(nnf, c))
            for v in get_free_vars(nnf, c)
                push!(s, mkLit(v, rand((true, false))))
            end
            return [c.child]
        else
            x -= cmc
        end
    end
end

function sample(nnf :: DDNNF, s :: Vector{Lit}, n :: AndNode)
    f(c) = c.child
    return map(f, n.children)
end

function sample(nnf :: DDNNF)
    stack = [Int64(1)]
    res = Vector{Lit}()

    while length(stack) > 0
        x = pop!(stack)
        y = sample(nnf, res, nnf.nodes[x])
        for i in y
            push!(stack, i)
        end
    end

    return res
end

function get_solution(nnf :: DDNNF, s :: Vector{Lit}, id :: BigInt, n :: TrueNode)
    return []
end

function get_solution(nnf :: DDNNF, s :: Vector{Lit}, id :: BigInt, n :: FalseNode)
    return []
end

function get_solution(nnf :: DDNNF, s :: Vector{Lit}, id :: BigInt, n :: UnaryNode)
    union!(s, get_literals(nnf, n.child))
    mc = get_mc(nnf, n)
    for v in get_free_vars(nnf, n.child)
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

function get_solution(nnf :: DDNNF, s :: Vector{Lit}, id :: BigInt, n :: OrNode)
    if id > get_mc(nnf, n)
        println("o ", id, " <= ", get_mc(nnf, i))
    end
    for c in n.children
        cmc = get_mc(nnf, c)

        if id <= cmc
            union!(s, get_literals(nnf, c))

            for v in get_free_vars(nnf, c)
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

function get_solution(nnf :: DDNNF, s :: Vector{Lit}, id :: BigInt, n :: AndNode)
    res = Vector{Tuple{BigInt, Int64}}()

    tmc = get_mc(nnf, n)
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

function get_solution(nnf :: DDNNF, id :: BigInt)
    stack = [(id, Int64(1))]
    res = Vector{Lit}()

    while length(stack) > 0
        lid, x = pop!(stack)
        y = get_solution(nnf, res, lid, nnf.nodes[x])
        for i in y
            push!(stack, i)
        end
    end

    return res
end

struct Edge
    b_lit :: Int64
    e_lit :: Int64
    b_free :: Int64
    e_free :: Int64

    child :: Int64
end

mutable struct AndNode
    children :: Vector{Edge}
    mc :: BigInt
end

mutable struct OrNode
    children :: Vector{Edge}
    mc :: BigInt
end

mutable struct UnaryNode
    child :: Union{Nothing, Edge}
    mc :: BigInt
end

struct TrueNode
end

struct FalseNode
end

const Node = Union{AndNode, OrNode, UnaryNode, TrueNode, FalseNode}

mutable struct DDNNF
    nodes :: Vector{Node}
    edges :: Vector{Edge}
    literals :: Vector{Lit}
    freev :: Vector{Var}
    ordering :: Vector{Int64}
    assumps :: Set{Lit}
end

function get_children(n :: AndNode)
    return n.children
end

function get_children(n :: OrNode)
    return n.children
end

function get_children(n :: UnaryNode)
    return [n.child]
end

function get_children(n :: TrueNode)
    return []
end

function get_children(n :: FalseNode)
    return []
end

function get_children(nnf :: DDNNF, i :: Int64)
    return get_children(nnf.nodes[i])
end

function add_edge(n :: AndNode, e :: Edge)
    push!(n.children, e)
end

function add_edge(n :: OrNode, e :: Edge)
    push!(n.children, e)
end

function add_edge(n :: UnaryNode, e :: Edge)
    n.child = e
end

function add_child(nnf :: DDNNF, src :: Int64, dst :: Int64, cnst :: AbstractArray{Lit}, free :: AbstractArray{Var})
    b_lit = length(nnf.literals) + 1
    for a in cnst
        push!(nnf.literals, a)
    end
    e_lit = length(nnf.literals)

    b_free = length(nnf.freev) + 1
    for a in free
        push!(nnf.freev, a)
    end
    e_free = length(nnf.freev)

    e = Edge(b_lit, e_lit, b_free, e_free, dst)
    add_edge(nnf.nodes[src], e)
end

function ddnnf_from_file(path :: String, prj = false, pvar = Set{Var}())
    res = DDNNF([], [], [], [], [], Set{Lit}())

    for line in eachline(path)
        if startswith(line, "o ")
            push!(res.nodes, OrNode([], 0))
        elseif startswith(line, "a ")
            push!(res.nodes, AndNode([], 0))
        elseif startswith(line, "u ")
            push!(res.nodes, UnaryNode(nothing, 0))
        elseif startswith(line, "t ")
            push!(res.nodes, TrueNode())
        elseif startswith(line, "f ")
            push!(res.nodes, FalseNode())
        elseif ! startswith(line, "c ")
            tmp = map(strip, split(line, ";"))
            p1 = split(tmp[1], " ")

            toint(x) = Base.parse(Int64, x)
            nonzero(x) = x != 0
            keep(x) = (! prj ) || (mkVar(x) in pvar)

            src = toint(p1[1])
            dst = toint(p1[2])

            cnst = filter(keep, map(mkLit, filter(nonzero, map(toint, p1[3:end]))))
            free = Vector{Var}()

            if length(tmp) == 2
                free = filter(keep, map(mkVar, filter(nonzero, map(toint, split(tmp[2], " ")))))
            end

            add_child(res, src, dst, cnst, free)
        end
    end

    compute_ordering(res)

    return res
end

function compute_ordering(nnf :: DDNNF)
    empty!(nnf.ordering)
    stack = [Int64(1)]
    visited = Set{Int64}()

    while length(stack) != 0
        num = stack[length(stack)]
        if ! (num in visited)
            getid(e :: Edge) = e.child

            all_done = true
            for c in map(getid, get_children(nnf.nodes[num]))
                if ! (c in visited)
                    all_done = false
                    push!(stack, c)
                end
            end

            if all_done
                pop!(stack)
                push!(visited, num)
                push!(nnf.ordering, num)
            end
        else
            pop!(stack)
        end
    end
end

get_free_vars(nnf :: DDNNF, i :: Edge) = @view nnf.freev[i.b_free : i.e_free]
get_literals(nnf :: DDNNF, i :: Edge) = @view nnf.literals[i.b_lit : i.e_lit]

# incomplete
function compute_free_var(nnf :: DDNNF)
    println("WARNING: usage of incomplete function compute_free_var")
    fv = Vector{Set{Var}}()
    for i in 1:length(nnf.nodes)
        push!(fv, Set{Var}())
    end

    for i in nnf.ordering
        for c in get_children(nnf, i)
            for l in nnf.literals[c.b_lit : c.e_lit]
                push!(fv[i], mkVar(l))
            end

            for v in fv[c.child]
                push!(fv[i], v)
            end
        end
    end

    m = 0
    for i in nnf.ordering
        if isa(nnf.nodes[i], UnaryNode)
            for c in get_children(nnf, i)
                println("error")
            end
        elseif isa(nnf.nodes[i], OrNode)
            for c in get_children(nnf, i)
                t2 = setdiff(fv[i], map(mkVar, nnf.literals[c.b_lit : c.e_lit]))
                setdiff!(t2, fv[c.child])
                t3 = Set(nnf.freev[c.b_free : c.e_free])

                if t2 != t3
                    m += 1
                    print("(", i, " -> ", c.child, ") ")
                    for i in t2
                        print(mkReadable(i), " ")
                    end
                    print(":")
                    for i in t3
                        print(mkReadable(i), " ")
                    end
                    print("\n   ")
                    for i in fv[i]
                        print(mkReadable(i), " ")
                    end
                    print("\n   ")
                    for i in fv[c.child]
                        print(mkReadable(i), " ")
                    end
                    print("\n   ")
                    for i in nnf.literals[c.b_lit : c.e_lit]
                        print(mkReadable(i), " ")
                    end
                    println("")
                end
            end
        end
    end
    println("m ", m)
end


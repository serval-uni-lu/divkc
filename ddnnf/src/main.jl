include("var.jl")

struct Edge
    b_lit :: Int64
    e_lit :: Int64
    b_free :: Int64
    e_free :: Int64

    child :: Int64
end

mutable struct AndNode
    mc :: BigInt
    children :: Vector{Edge}
end

mutable struct OrNode
    mc :: BigInt
    children :: Vector{Edge}
end

mutable struct UnaryNode
    mc :: BigInt
    child :: Edge
end

struct TrueNode
    mc :: BigInt

    TrueNode() = new(1)
end

struct FalseNode
    mc :: BigInt

    FalseNode() = new(0)
end

const Node = Union{AndNode, OrNode, UnaryNode, TrueNode, FalseNode}

struct DDNNF
    nodes :: Vector{Node}
    edges :: Vector{Edge}
    literals :: Vector{Lit}
    freev :: Vector{Var}
    ordering :: Vector{Int64}
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

function parse(path :: String)
    res = DDNNF([], [], [], [], [])

    for line in eachline(path)
        if startswith(line, "o ")
            push!(res.nodes, OrNode(0, []))
        elseif startswith(line, "a ")
            push!(res.nodes, AndNode(0, []))
        elseif startswith(line, "u ")
            push!(res.nodes, UnaryNode(0, 0))
        elseif startswith(line, "t ")
            push!(res.nodes, TrueNode())
        elseif startswith(line, "f ")
            push!(res.nodes, FalseNode())
        elseif ! startswith(line, "c ")
            tmp = map(strip, split(line, ";"))
            p1 = split(tmp[1], " ")

            toint(x) = Base.parse(Int64, x)
            nonzero(x) = x != 0

            src = toint(p1[1])
            dst = toint(p1[2])

            cnst = map(mkLit, filter(nonzero, map(toint, p1[3:end])))
            free = Vector{Var}()

            if length(tmp) == 2
                free = map(mkVar, filter(nonzero, map(toint, split(tmp[2], " "))))
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

function annotate_mc(nnf :: DDNNF)
    for i in nnf.ordering
        annotate_mc(nnf, nnf.nodes[i])
    end
end

function annotate_mc(nnf :: DDNNF, n :: FalseNode)
end

function annotate_mc(nnf :: DDNNF, n :: TrueNode)
end

function annotate_mc(nnf :: DDNNF, n :: UnaryNode)
    cmc = get_mc(nnf, c.child.child)
    n.mc = 2^(n.child.e_free - n.child.b_free + 1) * cmc
end

function annotate_mc(nnf :: DDNNF, n :: OrNode)
    n.mc = BigInt(0)

    for c in n.children
        cmc = get_mc(nnf, c.child)
        n.mc += 2^(c.e_free - c.b_free + 1) * cmc
    end
end

function annotate_mc(nnf :: DDNNF, n :: AndNode)
    n.mc = BigInt(1)

    for c in n.children
        cmc = get_mc(nnf, c.child)
        n.mc *= 2^(c.e_free - c.b_free + 1) * cmc
    end
end

get_mc(n) = n.mc
get_mc(nnf :: DDNNF, i :: Int64) = get_mc(nnf.nodes[i])

nnf = parse("res.nnf")
annotate_mc(nnf)
println(get_mc(nnf, 1))

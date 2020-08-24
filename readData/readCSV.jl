using DataFrames, CSV
using JuMP
using Gurobi

m = Model(solver = GurobiSolver())
@variable(m, tmpMat[1:2, 1:3])

test = CSV.read(joinpath(Pkg.dir("DataFrames"), "/Users/bobobo/Documents/fairSyn/juliaCode/readData/test.csv"))

println(head(test))

mt = convert(Matrix, test[:,1:3])

println(mt)

@constraint(m, readData[i=1:2, j=1:3], tmpMat[i,j] == mt[i,j])

println(readData)
#m = Matrix{Float64}(CSV.read("/Users/bobobo/Documents/fairSyn/juliaCode/readData/test.csv", header=0, delim=';'))



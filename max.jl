using JuMP
using Gurobi
m = Model(solver = GurobiSolver())
@variable(m, 0 <= x <= 2, Int)
@variable(m, 0 <= y <= 30, Int)

@objective(m, Max, 5x + 3*y)
@constraint(m, 1x + 5y <= 3)

print(m)

status = solve(m)

println("Objective Value: ", getobjectivevalue(m))
println("x = ", getvalue(x))
println("y =", getvalue(y))

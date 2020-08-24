using JuMP
using Gurobi
using DataFrames, CSV

m = Model(solver = GurobiSolver(NonConvex=2))

# set macro, the size of outputClass, input features, .. etc.
NE = 10; # number of examples
NF = 5; # number of input features
NO = 2; # number of output features

# formalize the level
nlevel = 2;
NL = 2^nlevel; # number of leaves
NB = (2^nlevel)-1; # number of branches

# assuming the number of classes is NO
@variable(m, Lt[1:NL], Int)

# bobo : In the one level setting, Am cannot be [0,0,0,..0] vector
@variable(m, 0 <= dt[1:NB] <= 1, Int)
@variable(m, Nt[1:NL] >= 0, Int)
@variable(m, Nkt[1:NL, 1:NO], Int)
# bobo : the range of Ckt should also be in [0,1]
@variable(m, 0 <= Ckt[1:NL, 1:NO] <= 1, Int)

@variable(m, n)
# n : number of examples (data points)
@constraint(m, n == NE)

# 3 constraints for the loss variable Lt
@constraint(m, consLoss1[i=1:NL, j=1:NO], Lt[i] >= Nt[i] - Nkt[i,j] - n*(1-Ckt[i,j]))
@constraint(m, consLoss2[i=1:NL, j=1:NO], Lt[i] <= Nt[i] - Nkt[i,j] + n*Ckt[i,j])
@constraint(m, consLoss3[i=1:NL], Lt[i] >= 0)

@variable(m, sumLt, Int)
@constraint(m, 0 <= sumLt)
@constraint(m, sumLt == sum(Lt[i] for i = 1:NL))

@variable(m, Yik[1:NE, 1:NO])
@variable(m, 0 <= Zit[1:NE, 1:(NL+NB)] <= 1, Int)

# constraint for the root node which has all the exmaples
@constraint(m, zitDef[i=1:NE], Zit[i, 1] == 1)

@constraintref def1[1:NL, 1:NO]
for t = 1:NL
	for k = 1:NO
		def1[t,k] = @constraint(m, Nkt[t,k] == sum((0.5*Zit[i,t+NB])*(1+Yik[i,k]) for i = 1:NE))
	end
end

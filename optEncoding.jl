using JuMP
using Gurobi
using DataFrames, CSV

m = Model(solver = GurobiSolver())

# set macro, the size of outputClass, input features, .. etc.
NE = 9; # number of examples
NF = 5; # number of input features
NO = 2; # number of output features

# assuming the number of classes is NO
@variable(m, Lt[1:2], Int)

# bobo : In the one level setting, Am cannot be [0,0,0,..0] vector
@variable(m, dt[1:1] == 1)
@variable(m, Ft[1:2])
@variable(m, Nt[1:2] >= 0, Int)
@variable(m, Nkt[1:2, 1:NO], Int)
# bobo : the range of Ckt should also be in [0,1]
@variable(m, 0 <= Ckt[1:2, 1:NO] <= 1, Int)

@variable(m, n)
# Here we assume n being 9
# the number of leaves (t) is 2
# t: represents the leaves node
# b: represents the branch node

# 3 constraints for the loss variable Lt

@constraint(m, n == NE)
@constraint(m, consLoss1[i=1:2, j=1:NO], Lt[i] >= Nt[i] - Nkt[i,j] - n*(1-Ckt[i,j]))
@constraint(m, consLoss2[i=1:2, j=1:NO], Lt[i] <= Nt[i] - Nkt[i,j] + n*Ckt[i,j])
@constraint(m, consLoss3[i=1:2], Lt[i] >= 0)

@variable(m, sumLt, Int)
@constraint(m, 0 <= sumLt)
@constraint(m, sumLt == sum(Lt[i] for i = 1:2))
# check the constraints
#for i = 1:2
#    for j = 1:NO
#        println(consLoss1[i,j])
#    end
#end


# n : number of examples (data points)
# Here we assume n being 9
@variable(m, Yik[1:NE, 1:NO])
@variable(m, 0 <= Zit[1:NE, 1:2] <= 1, Int)
@constraintref def1[1:2, 1:NO]
for t = 1:2
    for k = 1:NO
        def1[t,k] = @constraint(m, Nkt[t,k] == sum((0.5*Zit[i,t])*(1+Yik[i,k]) for i = 1:NE))
    end
end

#@constraint(m, def1[t=1:2, k=1:NO], Nkt[t,k] == sum(0.5*Zit[i,t]*(1+Yik[i,k]) for i =1:NO))

@constraint(m, defNt[t=1:2], Nt[t] == sum(Zit[i,t] for i = 1:NE))

@variable(m, 0 <= lt[1:2] <= 1, Int)
@constraint(m, deflt[t=1:2], lt[t] == sum(Ckt[t,k] for k = 1:NO))

#we consider the basic case when there is one branch node and two leaves node
@variable(m, 0 <= Am[1:NF, 1:1] <= 1, Int)
# Bm needs to be updated later
@variable(m, Bm, Int)
#we assume the input data table has 5 boolean features
@variable(m, Xi[1:NE, 1:NF])

@variable(m, tmpRes[1:NE, 1:1])
@constraint(m, Xi * Am .== tmpRes)
@constraint(m, ambm[i=1:NE, t=2], tmpRes[i, 1] >= Bm - (1 - Zit[i,t]))
#@constraint(m, XivecTmp[k=1:NF], vecTmp[k,1] == Xi[3, k])

@variable(m, epilsonMax)
@variable(m, epilson[1:NF])
@variable(m, XiPlus[1:NE, 1:NF])
@variable(m, tmpRes2[1:NE, 1:1])

# epilson and epilson should be computed offline since the result only depdends on the input dataset which is already given.

@constraint(m, epilsonDef[i=1:NF], epilson[i] == 1)
@constraint(m, epilsonMax == 1)

@constraint(m, XiPlusDef[i=1:NE, j=1:NF], XiPlus[i,j] == Xi[i,j] + epilson[j])
@constraint(m, XiPlus * Am .== tmpRes2)
@constraint(m, ambm2[i=1:NE, t=1], tmpRes2[i,1] <= Bm + (1 + epilsonMax)*(1 - Zit[i,t]))

@constraint(m, ZitSum[i=1:NE], 1 == sum(Zit[i,t] for t = 1:2))

@constraint(m, ZitLt[i=1:NE, t=1:2], Zit[i,t] <= lt[t])

@variable(m, Nmin)
@constraint(m, ZitLt2[t=1:2], sum(Zit[i,t] for i = 1:NE) >= Nmin * lt[t])

# Set N_min as 1
@constraint(m, Nmin == 1)

# t here represents the root/branch node (t = 0)
@constraint(m, ajtDt, dt[1] == sum(Am[i,1] for i = 1:NF))


# t represets branch here
@constraint(m, Bm >= 0)
@constraint(m, dt[1] >= Bm)

# now we only consider two levels, so we emit constraint dt <= d_{p(t)}

#read the input data from CSV file

Dataset = CSV.read(joinpath(Pkg.dir("DataFrames"), "/Users/bobobo/Documents/fairSyn/juliaCode/readData/test.csv"))
# Data matrix without Dataset
mt = convert(Matrix, Dataset[:,1:NF])
# Data matrix with output
output = convert(Vector, Dataset[:, (NF+1)])

@constraint(m, readData[i=1:NE, j=1:NF], Xi[i,j] == mt[i,j])

# definition of Yik
@constraint(m, YikDef0[i=1:NE, j=1:NO ; output[i] == (j - 1)], Yik[i,j] == 1)
@constraint(m, YikDef1[i=1:NE, j=1:NO ; output[i] != (j - 1)], Yik[i,j] == -1)


@objective(m, Min, sumLt)

solve(m)

println("Final Solution: [ Yik : $(getvalue(Yik))]")
println("Final Solution: [ Nt : $(getvalue(Nt))]")
println("Final Solution: [ Lt : $(getvalue(Lt))]")
println("Final Solution: [ Zit : $(getvalue(Zit))]")
println("Final Solution: [ Am : $(getvalue(Am))  ]")
println("Final Solution: [ Bm : $(getvalue(Bm))  ]")

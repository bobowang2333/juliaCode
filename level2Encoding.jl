using JuMP
using Gurobi
using DataFrames, CSV

m = Model(solver = GurobiSolver())

# set macro, the size of outputClass, input features, .. etc.
NE = 10; # number of examples
NF = 5; # number of input features
NO = 2; # number of output features
NL = 4; # number of leaves
NB = 3; # number of branches

# assuming the number of classes is NO
@variable(m, Lt[1:NL], Int)

# bobo : In the one level setting, Am cannot be [0,0,0,..0] vector
@variable(m, dt[1:NB], Int)
#@variable(m, Ft[1:2])
@variable(m, Nt[1:NL] >= 0, Int)
@variable(m, Nkt[1:NL, 1:NO], Int)
# bobo : the range of Ckt should also be in [0,1]
@variable(m, 0 <= Ckt[1:NL, 1:NO] <= 1, Int)

@variable(m, n)
# Here we assume n being 9
# the number of leaves (t) is 2
# t: represents the leaves node
# b: represents the branch node

# 3 constraints for the loss variable Lt

@constraint(m, n == NE)
@constraint(m, consLoss1[i=1:NL, j=1:NO], Lt[i] >= Nt[i] - Nkt[i,j] - n*(1-Ckt[i,j]))
@constraint(m, consLoss2[i=1:NL, j=1:NO], Lt[i] <= Nt[i] - Nkt[i,j] + n*Ckt[i,j])
@constraint(m, consLoss3[i=1:NL], Lt[i] >= 0)

@variable(m, sumLt, Int)
@constraint(m, 0 <= sumLt)
@constraint(m, sumLt == sum(Lt[i] for i = 1:NL))
# check the constraints
#for i = 1:2
#    for j = 1:NO
#        println(consLoss1[i,j])
#    end
#end

# n : number of examples (data points)
# Here we assume n being 9
@variable(m, Yik[1:NE, 1:NO])
@variable(m, 0 <= Zit[1:NE, 1:(NL+NB)] <= 1, Int)

# constraint for the root node which has all the exmaples
@constraint(m, zitDef[i=1:NE], Zit[i, NL+NB] == 1)

@constraintref def1[1:NL, 1:NO]
for t = 1:NL
    for k = 1:NO
        def1[t,k] = @constraint(m, Nkt[t,k] == sum((0.5*Zit[i,t])*(1+Yik[i,k]) for i = 1:NE))
    end
end

#@constraint(m, def1[t=1:2, k=1:NO], Nkt[t,k] == sum(0.5*Zit[i,t]*(1+Yik[i,k]) for i =1:NO))

@constraint(m, defNt[t=1:NL], Nt[t] == sum(Zit[i,t] for i = 1:NE))

@variable(m, 0 <= lt[1:NL] <= 1, Int)
@constraint(m, deflt[t=1:NL], lt[t] == sum(Ckt[t,k] for k = 1:NO))

#we consider the basic case when there is one branch node and two leaves node
@variable(m, 0 <= Am[1:NF, 1:NB] <= 1, Int)

# bobo  : we assume f1 represents the sensitive feature, which cannot be used to split examples
#@constraint(m, Am[1, 1] == 0)
@constraintref sensitiveFeature[1:NB]
for i = 1:NB
    sensitiveFeature[i] = @constraint(m, Am[1, i] == 0)
end

# Bm needs to be updated later
@variable(m, 0 <= Bm[1:NB] <= 1, Int)
#we assume the input data table has 5 boolean features
@variable(m, Xi[1:NE, 1:NF])

@variable(m, tmpRes[1:NE, 1:NB])
@constraint(m, Xi * Am .== tmpRes)

##### constraint for right branch
@constraint(m, ambmR0[i=1:NE, t=6, b=1], tmpRes[i, b] >= Bm[b] - (1 - Zit[i,t]))
@constraint(m, ambmR1[i=1:NE, t=3, b=1], tmpRes[i, b] >= Bm[b] - (1 - Zit[i,t]))
@constraint(m, ambmR2[i=1:NE, t=4, b=1], tmpRes[i, b] >= Bm[b] - (1 - Zit[i,t]))

#### constraint for the right leaves
@constraint(m, ambmR3[i=1:NE, t=2, b=2], tmpRes[i,b] >= Bm[b] - (1 - Zit[i,t]))
@constraint(m, ambmR4[i=1:NE, t=4, b=3], tmpRes[i,b] >= Bm[b] - (1 - Zit[i,t]))

#@constraint(m, XivecTmp[k=1:NF], vecTmp[k,1] == Xi[3, k])


#### constraint for left side
@variable(m, epilsonMax)
@variable(m, epilson[1:NF])
@variable(m, XiPlus[1:NE, 1:NF])
@variable(m, tmpRes2[1:NE, 1:NB])

# epilson and epilson should be computed offline since the result only depdends on the input dataset which is already given.

@constraint(m, epilsonDef[i=1:NF], epilson[i] == 1)
@constraint(m, epilsonMax == 1)

@constraint(m, XiPlusDef[i=1:NE, j=1:NF], XiPlus[i,j] == Xi[i,j] + epilson[j])
@constraint(m, XiPlus * Am .== tmpRes2)

#### constraint for left branch
@constraint(m, ambmL0[i=1:NE, t=5, b=1], tmpRes2[i,b] <= Bm[b] + (1 + epilsonMax)*(1 - Zit[i,t]))
@constraint(m, ambmL1[i=1:NE, t=1, b=1], tmpRes2[i,b] <= Bm[b] + (1 + epilsonMax)*(1 - Zit[i,t]))
@constraint(m, ambmL2[i=1:NE, t=2, b=1], tmpRes2[i,b] <= Bm[b] + (1 + epilsonMax)*(1 - Zit[i,t]))

#### constraints for left leaves
@constraint(m, ambmL3[i=1:NE, t=1, b=2], tmpRes2[i,b] <= Bm[b] + (1 + epilsonMax)*(1 - Zit[i,t]))
@constraint(m, ambmL4[i=1:NE, t=3, b=3], tmpRes2[i,b] <= Bm[b] + (1 + epilsonMax)*(1 - Zit[i,t]))

# horizontal dependency
@constraint(m, ZitSum[i=1:NE], 1 == sum(Zit[i,t] for t = 1:NL))
@constraint(m, ZitSum2[i=1:NE], 1 == sum(Zit[i,t] for t = 5:6))

# Sum{Zit[children]} == Zit[root]  vertical dependency
@constraint(m, parentChild0[i=1:NE], Zit[i,7] == Zit[i,5] + Zit[i,6])
@constraint(m, parentChild1[i=1:NE], Zit[i,5] == Zit[i,1] + Zit[i,2])
@constraint(m, parentChild2[i=1:NE], Zit[i,6] == Zit[i,3] + Zit[i,4])


@constraint(m, ZitLt[i=1:NE, t=1:NL], Zit[i,t] <= lt[t])

@variable(m, Nmin)
@constraint(m, ZitLt2[t=1:NL], sum(Zit[i,t] for i = 1:NE) >= Nmin * lt[t])


# Set N_min as 1
@constraint(m, Nmin == 1)

# t here represents the root/branch node (t = 0)
@constraint(m, ajtDt[j=1:NB], dt[j] == sum(Am[i,j] for i = 1:NF))


# t represets branch here
#@constraint(m, Bm[1:NB] .>= 0)
@constraint(m, dtBmDef[i=1:NB], dt[i] >= Bm[i])

# now we only consider two levels, so we add constraint dt <= d_{p(t)}

@constraint(m, dt[1] >= dt[2])
@constraint(m, dt[1] >= dt[3])

@variable(m, sumDt)
@constraint(m, sumDt == sum(dt[i] for i = 1:NB))


# If Am and Bm being both 0, then Zit[i,t] == Zit[i, LeftChild(t)]
# first branch (7->5), if branch is empty, then all node goes to the right side
@constraint(m, empty1[i=1:NE, child = 6, parent = 7], Zit[i, parent] - Zit[i, child] <= dt[1])
# second branch
@constraint(m, empty2[i=1:NE, child = 2, parent = 5], Zit[i, parent] - Zit[i, child] <= dt[2])
# third branch
@constraint(m, empty3[i=1:NE, child = 4, parent = 6], Zit[i, parent] - Zit[i, child] <= dt[3])

#=
for i = 1:NE
    child = 4
    parent = 6
    println(empty3[i, child, parent])
end
=#

#read the input data from CSV file

Dataset = CSV.read(joinpath(Pkg.dir("DataFrames"), "/Users/bobobo/Documents/fairSyn/juliaCode/readData/test2.csv"))
# Data matrix without Dataset
mt = convert(Matrix, Dataset[:,1:NF])
# Data matrix with output
output = convert(Vector, Dataset[:, (NF+1)])

@constraint(m, readData[i=1:NE, j=1:NF], Xi[i,j] == mt[i,j])

# definition of Yik
@constraint(m, YikDef0[i=1:NE, j=1:NO ; output[i] == (j - 1)], Yik[i,j] == 1)
@constraint(m, YikDef1[i=1:NE, j=1:NO ; output[i] != (j - 1)], Yik[i,j] == -1)

# add fairness constraints
# FPt represents the number of data points, which is female and the output class is positive.
# Ft represents the number of data points, which is female. Similarly for MPt and Mt
@variable(m, FPt[1:NE, 1:NL] >= 0, Int)
@variable(m, Ft[1:NE, 1:NL] >= 0, Int)
@variable(m, MPt[1:NE, 1:NL] >= 0, Int)
@variable(m, Mt[1:NE, 1:NL] >= 0, Int)

@constraint(m, defFPt[i=1:NE, t=1:NL; Zit[i,t] == 1 && output[i] == 1 && Xi[i,1] == 0], FPt[i,t] == 1)
@constraint(m, defFPt1[i=1:NE, t=1:NL; !(Zit[i,t] == 1 && output[i] == 1 && Xi[i,1] == 0)], FPt[i,t] == 0)

@constraint(m, defMPt[i=1:NE, t=1:NL; Zit[i,t] == 1 && output[i] == 1 && Xi[i,1] == 1], MPt[i,t] == 1)
@constraint(m, defMPt1[i=1:NE, t=1:NL; !(Zit[i,t] == 1 && output[i] == 1 && Xi[i,1] == 1)], MPt[i,t] == 0)

@constraint(m, defFt[i=1:NE, t=1:NL; Zit[i,t] == 1 && Xi[i,1] == 0], Ft[i,t] == 1)
@constraint(m, defFt1[i=1:NE, t=1:NL; !(Zit[i,t] == 1 && Xi[i,1] == 0)], Ft[i,t] == 0)

@constraint(m, defMt[i=1:NE, t=1:NL; Zit[i,t] == 1 && Xi[i,1] == 1], Mt[i,t] == 1)
@constraint(m, defMt1[i=1:NE, t=1:NL; !(Zit[i,t] == 1 && Xi[i,1] == 1)], Mt[i,t] == 0)

@variable(m, SumFPt[1:NL] >= 0, Int)
@variable(m, SumFt[1:NL] >= 0, Int)
@variable(m, SumMPt[1:NL] >= 0, Int)
@variable(m, SumMt[1:NL] >= 0, Int)

@constraint(m, defSumFPt[t=1:NL], SumFPt[t] == sum(FPt[i, t] for i = 1:NE))
@constraint(m, defSumFt[t=1:NL], SumFt[t] == sum(Ft[i, t] for i = 1:NE))
@constraint(m, defSumMPt[t=1:NL], SumMPt[t] == sum(MPt[i, t] for i = 1:NE))
@constraint(m, defSumMt[t=1:NL], SumMt[t] == sum(Mt[i, t] for i = 1:NE))

@variable(m, fairVar[1:NL])
#@NLconstraint(m, fair1[t=1:2], fairVar[t] == (SumFPt[t]/SumFt[t] - 0.8 * SumMPt[t]/SumMt[t]))

# linear fairness constraints
@constraint(m, fairDef[t=1:NL], fairVar[t] == SumFPt[t] * SumMt[t] - 0.8 * SumMPt[t] * SumFt[t])

@variable(m, fair)
@constraint(m, fair == 0.5 * sum(fairVar[t] for t = 1:NL))

#@objective(m, Min, sumLt - fair)

#@objective(m, Max, fair)

@objective(m, Min, sumLt - fair)
#optimize!(m)

solve(m)

# compute the infeasible constraints set if the solver outputs no result
#=
grb_model = m.internalModel.inner
num_constrs = Gurobi.num_constrs(grb_model)
Gurobi.computeIIS(grb_model)
iis_constrs = Gurobi.get_intattrarray(grb_model, "IISConstr",  1, num_constrs)
m.linconstr[find(iis_constrs)]
=#

println("Final Solution: [ sumDt : $(getvalue(sumDt))]")
println("Final Solution: [ sumLt : $(getvalue(sumDt))]")
println("Final Solution: [ Yik : $(getvalue(Yik))]")
println("Final Solution: [ Nt : $(getvalue(Nt))]")
println("Final Solution: [ Lt : $(getvalue(Lt))]")
println("Final Solution: [ Zit : $(getvalue(Zit))]")
println("Final Solution: [ Am : $(getvalue(Am))  ]")
println("Final Solution: [ Bm : $(getvalue(Bm))  ]")


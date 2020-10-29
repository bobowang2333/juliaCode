#README

We implemented the Overhead-aware Learning for Fair and Approximate Optimal Decision Tree Classifier, which is encoded in Julia 1.5.1 along with the Gurobi Solver 9.03. 

##Installation
First download and install [Julia](https://julialang.org/downloads/) and open the Julia notebook. From Julia, [JuMP](https://jump.dev/JuMP.jl/v0.21.1/installation/) is installed by built-in package manager.

```bash
import Pkg
Pkg.add("JuMP")
```
We also use [Gurobi](https://www.gurobi.com/downloads/gurobi-optimizer-eula/) Solver with a JuMP model, as the following:

```bash
import Pkg
Pkg.add("Gurobi")
using JuMP
using Gurobi
model = Model(Gurobi.Optimizer)
```

The Gurobi solver offers free academic license and you just need to register your educational account.

## Running Instruction
Here we document all the running instructions to learn a fair decision tree by iteratively invoking the solver. In addition, we will also compare with the greedy algorithm (CART) to learn the decision tree.

**Dataset Preprocessing** 

The input datasets are pretty arbitrary, the value may range over boolean, real, categorical domains. We will transform the original dataset to the boolean dataset and present two different processing approaches. First, for the categorical value, we adopt the one-hot encoding to generate multiple boolean columns for the replacement. Second, for the read-valued features, we require user to specify the config file denoting the range to split for each continuous-valued features.

```bash
python ./Dataset/transDataset.py original_dataset configuration_file sensitive_index 0
```

After executing the above command, it will output the booleanized dataset in the name <mark>"boolean.data"</mark>. The following shows one example of the configuration file (in [Germany](https://archive.ics.uci.edu/ml/datasets/Statlog+(German+Credit+Data)) Dataset):

```bash
1 12 24 36 48
4 3000 6000 9000
12 20 40 60
```

The format of configuration file: each line of the file corresponds to each feature, where the first number denote the index of that feature (e.g. 4th feature), and the remaining three values (e.g. 3000, 6000, 9000) represent three thresholds which will be used to split the original feature column. 

Next, we will generate the CSV file (with header) and sample part of the datasets as our training set, where the input is <mark>"boolean.data"</mark>. 

```bash
python ./Dataset/data2CSV.py boolean.data output_CSV sample_para test_CSV
```

In the above command, if sampl\_para is 0, which means that we do not sample the data and generate the origianl dataset in CSV form. Otherwise, if sample_para is 1, which would sample the data in 1/50 size. (50 is the current setting, and we can modify it later on)

**MIP-Iter Running**

We will talk about how to encode the decision tree learning in mixed integer optimization framework and run the encoding iteratively to select the splitting features. Here we name the training dataset as <mark>test1.csv</mark> before running the script.

```bash
./runEncodingHighNew.sh -f 1 -r RunningPath -j juliaBinary -s senID -n nlevel -N featureNum
```
In the above, _-f_ denotes the number of dataset where we start to build the tree. For instance, we use <mark>test1.csv</mark> as the training dataset, so we should input _-f 1_. As the learner keeps spliting the tree, the bash script will automatically generate test2.csv, test3.csv,...etc. 
_-j_ denotes the julia binary position.
_-s_ denotes the index of the sensitive feature and _-n_ denotes the number of levels in each MIP iteration.
<table>
	<tbody>
		<tr>
			<th>para</th>
			<th align="center">Description</th>
		</tr>
		<tr>
			<td>-f</td>
			<td> the index of the training dataset (the start)</td>
		</tr>
		<tr>
			<td>-j</td>
			<td> the path of the Julia Binary</td>
		</tr>
		<tr>
			<td>-s</td>
			<td>the index of the sensitive feature</td>
		</tr>
		<tr>
			<td>-n</td>
			<td>the number of levels which will be used to encode the learner</td>
		</tr>
		<tr>
			<td>-N</td>
			<td>the number of features of the dataset</td>
		</tr>
		<tr>
			<td>-r</td>
			<td>running path, usually is the path of the dataset</td>
		</tr>
	</tbody>
</table>
_-r_ indicates the running path where it should include the following files:

<table>
  <tbody>
    <tr>
      <th>File Name</th>
      <th align="center">Description</th>
    </tr>
    <tr>
      <td>countCSV.py</td>
      <td align="center">Calculate the statistics of the dataset to provide information to generate the encoding</td>
    </tr>
    <tr>
      <td>genJulia</td>
      <td align="center">The binary file which takes the statistics of the original dataset as input, and outputs the MIP encoding julia file with the name of <mark>level.jl</mark> </td>
    </tr>
    <tr>
    	<td>readRes</td>
    	<td>Binary file which reads the output of the Gurobi solver and outputs the splitting feature information</td>
    </tr>
    <tr>
    	<td>splitGroupHigh.py</td>
    	<td>Split the current dataset by the splitting feature at each iteration</td>
    </tr>
	</tbody>
</table>

The following command is an example to run the script. 
```bash
./runEncodingHighNew.sh -d 1 -r ~/Downloads/juliaCode/Dataset/germany -j /Applications/Julia-1.5.app/Contents/Resources/julia/bin -s 13 -n 2 -N 20
```

The above command will also output the result file <mark>treeRes.txt</mark>, it documents the mapping from branch to the splitting feature.
After finishing the running, we may also want to analyze the result of the learner (<mark>treeRes.txt</mark>) by analyzing its accuracy and fairness value.

```bash
python readResQuan.py treeRes.txt trainSet testSet OutputCSV
```
The above command takes the learner result, training set and test set to generate the predicted result for the test set. The format of the output file is in CSV form. Here, _trainSet_ denotes the path of the training set, same for _testSet_. The following command will calculates the [DTDI](https://aaai.org/ojs/index.php/AAAI/article/view/3943) value to measure the discrimination level (the lower, the less disciminative)

```bash
python getDTDI.py OutputCSV 0 1
```

**Compare with CART**

We may also want to compare with the standard CART algorithms and calculate its 
discrimination value as well, which can be achieved by running the following command.
Here, we assume the input dataset is in the <mark>.data</mark> format (same as the format of [Germany](https://archive.ics.uci.edu/ml/datasets/Statlog+(German+Credit+Data)) Dataset), each row of which is the feature vector of each example.

```bash
python getDTDI.py output_CSV senID 0
```

In the above command, senID repsents the index of the sensitive feature. After committing this command, we will get the output DTDI value of CART-learned tree.

**Quantiative Dataset**

We can accept not only the boolean dataset but also the quantiative dataset. For the quantiative datset processing, we also handle them in two ways. First, for categorical value, we also adopt the one-hot encoding. Second, for continoused value, we normalize the feature value of that column to range [0,1] as the following: (assume we are handling feature F)

F_v = ( F_v - F_min ) / ( F_max-F_min ) 

```bash
python ./Dataset/transDataset.py original_dataset 0 sensitive_index 1
```
Running the above command, we can transform the original dataset to the quantiative dataset where each feature value is from the range [0,1].

**Note**

Before we execute the whole running script, we have to delete the treeRes.txt (in the running path).

You can run the following command to finish the MIP-iter running in one shot for quantiative dataset:

```bash
./runWhole.sh -f /Users/jingbow/Downloads/juliaCode/Dataset/testScript/german.data -c /Users/jingbow/Downloads/juliaCode/Dataset/testScript/config -s 12 -r /Users/jingbow/Downloads/juliaCode/Dataset/testScript -j /Applications/Julia-1.5.app/Contents/Resources/julia/bin -N 20 -n 2 -Q 1
```

_-f_ represents the path of the specific dataset, _-c_ represents the directory of the dataset, _-s_ represents the sensitive ID, _-r_ represents the directory of the specific dataset, _-j_ represents the path of Julia binary, _-N_ represents the number of features, _-n_ represent the number of tree levels in the Encoding and _-Q_ represents to run the Encoding in quantiative dataset or boolean dataset.

You can run the following command to finish the MIP-iter running in one shot for boolean dataset:

```bash
./runWhole.sh -f /Users/jingbow/Downloads/juliaCode/Dataset/testScript/german.data -c /Users/jingbow/Downloads/juliaCode/Dataset/testScript/German.config -s 12 -r /Users/jingbow/Downloads/juliaCode/Dataset/testScript -j /Applications/Julia-1.5.app/Contents/Resources/julia/bin -N 20 -n 2 -Q 0
```
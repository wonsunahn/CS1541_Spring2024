#!/bin/bash

bash compile.sh

java -cp bin BranchExperiment 10000000 sort

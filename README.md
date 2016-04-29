This repository is created in order to present the work held for the ns3 project assignment of the wireless networking course in TU Delft.

The final graphs are produced by the tool gnuplot. In order for the graps to be produced in every machine gnuplot should be installed. This is done by executing the following commands:

sudo apt-get update
sudo apt-get install gnuplot

For the first scenario the scripts "dist.sh", "0.pd", "5.pd", "10.pd" as well as the "read" executable program, need to be placed inside the working ns3 directory (/home/source/ns-3.24). In addition the source code for the first scenario named "wifi0.cc" needs to be placed in the scratch directory.

Finally running the command inside the ns3 working directory:

sh ./dist.sh

will execute all simulations and scripts and finally will produce the three graphs that are also shown in the report. This procedure is expected to last from fourty minutes to one hour!

For the second scenario the scripts "nd_ps.sh", "mac.pd", "phy.pd" as well as the "read1" executable program, need to be placed inside the working ns3 directory (/home/source/ns-3.24). In addition the source code for the second scenario named "wifi1.cc" needs to be placed in the scratch directory.

Finally running the command inside the ns3 working directory:

sh ./nd_ps.sh

will execute all simulations and scripts and finally will produce the three graphs that are also shown in the report. This procedure is expected to last for long time as well!

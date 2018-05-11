==============================================
LaMEM - Lithosphere and Mantle Evolution Model

A parallel 3D numerical code that can be used to model various thermomechanical 
geodynamical processes such as mantle-lithosphere interaction for rocks 
that have visco-elasto-plastic rheologies. The code is build on top of 
PETSc and the current version of the code uses a marker-in-cell 
approach with a staggered finite difference discretization. 

A range of (Galerkin) multigrid and iterative solvers are 
available, for both linear and non-linear rheologies, using Picard and 
quasi-Newton solvers (provided through the PETSc interface).

LaMEM has been tested on over 458'000 cores.


The current version is developed by
  Anton Popov       (Johannes Gutenberg University Mainz, popov@uni-mainz.de), 2011-
  Boris Kaus        (JGU Mainz, kaus@uni-mainz.de), 2011-
  Tobias Baumann    (JGU Mainz), 2011-
  Adina P�s�k       (JGU Mainz), 2012-
  Naiara Fernandez  (JGU Mainz), 2011-2014
  Arthur Bauville   (JGU Mainz), 2015

Older versions of LaMEM included a finite element solver as well, 
and were developed by:

  Boris J.P. Kaus (ETH Zurich, Switzerland). 2007-2011
  Dave A. May     (ETH Zurich, Switzerland).     2009-2011


The current version is compatible with PETSc version 3.7.x

Development work was supported by the European Research Council, 
with ERC Starting Grant 258830.

LaMEM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

LaMEM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LaMEM. If not, see <http://www.gnu.org/licenses/>.



0. Directory structure
----------------------
doc		-	Some documentation (remains incomplete)
matlab 		-	Various matlab files to change initial mesh and read binary output into matlab.
scripts 	-	Various scripts, currently mainly related to Paraview VTK files.
src		-	LaMEM source code; compile with "make mode=opt all"
tests		-	Directory with various benchmarks. 
utils		-	Various non-essential files.
bin             -       Contains binaries after compilation (/deb contains the debug version and /opt the optimized)
input_models 	-       Various input models (run with ../bin/LaMEM -ParamFile *.dat) TO BE UPDATED TO LATEST VERSION OF LAMEM



1. Dependencies
---------------

Essential dependencies-
  i) PETSc 3.7.7, ideally installed with the external packages SUPERLU_DIST, MUMPS and PASTIX
  
1) Dependency Installation
- i) PETSc: 
     See http://www.mcs.anl.gov/petsc/petsc-as/documentation/installation.html
     for installation instructions.
     We also provide detailed installation instructions on how to compile 
     PETSc (and mpich, gcc, as well as PETSc with the eclipse debugger environment)
     on various machines in /doc/installation

2) Building LaMEM
- Set the environment variable
     export PETSC_OPT=DIRECTORY_FOR_OPTIMIZED_PETSC
     export PETSC_DEB=DIRECTORY_FOR_DEBUG_PETSC 

- To build the source in the /src directory
     make mode=opt all

- Once build, you can verify that it works with:
    cd /tests
    make test

3) Visualize results
   We usually use PARAVIEW for this; a serial binary version can be downloaded 
   from www.paraview.org. Instructions on how to use a parallel version are in /doc
   Simply open the *.pvd files in paraview and push the play button  

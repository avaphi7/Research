# Research
A collection of code and materials related to analyzing proton-proton collision data from the Large Hadron Collider (LHC). The primary focus is on exercises involving transverse momenta (pT), the invariant mass distribution, and the creation of histograms. 

## Code Description
### 1. RandomTTree.C
This script generates a random TTree, fills it with generated values, and saves the TTree to a ROOT file.

### 2. CalculateHT.C
This script calculates HT and HTCentral (scalar sum of jets with |η| < 2.5) from proton-proton collision data.

### 3. photonInvariantMass.C
This script creates a histogram for the invariant mass of two photons.

### 4. cutflow.C
This script runs proton-proton collision data through a series of criteria and fills a histogram.

### 5. Higgs.C
This script analyzes the invariant mass distribution of final-state particles produced in proton-proton collisions.

### 6. Crystalball.C
This script fits the Higgs boson signal production channels with a double-sided crystal ball fit and saves the parameters.

### 7. HiggsFit.C
This script fits the signal data with a double-sided crystal ball fit and fits the background data with a Bernstein polynomial fit and plots in a histogram.

#### Workflow
Adding files:
- `git add [filename]`
- `git commit -m"[descriptive message]"`
- `git push personal main`

Retrieving files from Github
- do the "adding files" section above
- ensure that there is nothing to commit by typing `git status`
- `git pull personal main`

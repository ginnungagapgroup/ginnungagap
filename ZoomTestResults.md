

# Simple 2 level zoom #

The setup is 2-level: 256<sup>3</sup> and 512<sup>3</sup> DM particles. The lower resolution grid is used to create an initial mask. The box size is 500 h<sup>-1</sup> Mpc and the mask covers a sphere of R=100 h<sup>-1</sup> Mpc at the box center.

Here is the relevant part of the .ini files (the output filenames are omitted).
```
[Ginnungagap]
dim1D = 512
boxsizeInMpch = 500
zInit = 60.0
gridName = testGrid
normalisationMode = sigma8
[Cosmology]
modelOmegaRad0 = 8.348451673443855e-05
modelOmegaLambda0 = 0.692885
modelOmegaMatter0 = 0.307115
modelOmegaBaryon0 = 0.048206
modelHubble = 0.6777
modelSigma8 = 0.8288
modelNs = 0.963
modelTempCMB = 2.725
powerSpectrumFileName = PlanckN_g.dat
[WhiteNoise]
useFile = true
dumpWhiteNoise = false
readerSection = WhiteNoiseReader
[WhiteNoiseReader]
type = grafic
prefix = wn_512
qualifier = _delta
isWhiteNoise = true
[MPI]
# MPI will decide how to arrange the available processors over the y
# and z dimension.
nProcs = 1 0 0

[GenerateICs]
ginnungagapSection = Ginnungagap
doGas = false
doLongIDs = false
bufferSection = Buffer
inputSection = GenicsInput
outputSection = GenicsOutput
cosmologySection = Cosmology
maskSection = Mask
hierarchySection = Hierarchy
zoomLevel = 8
typeForLevel7 = 2
typeForLevel8 = 1

[Mask]
maskLevel = 7
minLevel = 7
maxLevel = 8
tileLevel = 1
readerType = legacy
readerSection = Lare

[Hierarchy]
numLevels = 9
minDim1D = 2
factor = 2

[Lare]
hasHeader = false
fileName = lare_sphere.dat
ngrid = 256 256 256

[GenicsOutput]
numFilesForLevel7 = 1
numFilesForLevel8 = 1
prefix = gz2lev

```
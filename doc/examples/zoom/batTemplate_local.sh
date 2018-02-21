refBatCreator ()
{
template="
./refineGrid ref_x_$m.ini
./refineGrid ref_z_$m.ini
./refineGrid ref_y_$m.ini
"
if [ $doDelta == 'true' ]; then
    template="$template
./refineGrid ref_d_$m.ini
"
fi
batwriter $1
}

refCutWNBatCreator ()
{
template="
./refineGrid ref_wn_cut_$mprev.ini
"
batwriter $1	
}

rscBatCreator ()
{
template="
./realSpaceConstraints $rscIni
"
batwriter $1
}

ggpBatCreator ()
{
template="
./ginnungagap $ggpIni
"
batwriter $1
}

genicsBatCreator ()
{
template="
./generateICs $genIni
"
batwriter $1
}

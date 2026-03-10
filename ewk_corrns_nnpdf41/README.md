# ewk_corrns_nnpdf41

This folder contains the material to generate EWK K-factors using madgraph
(v6.7). All the madgraph input is contained in folders called with the same
name as the relevant nnpdf4.1 data sets, which are placed in folders called
`<process_name>_ewk_corrns`. The user has to take the following actions.
- Go into the `<process_name>_ewk_corrns` folder and run
`./copy.sh`
`./run_all.sh`
Individual data sets can be run with the `run.sh` script within each data set
folder. The commands above generate pineappl grids that incorporate EW and
mixed EW-QCD corrections.
- Run the script `./validate.py`. This will validate the generated pineappl grids against theory 41_000_000. 
- Run the script `./compute_Kfact.py`. This will generate a folder, called
`K-factors` that is going to contain the K-factors. These can be
finally pasted in the relevant theories.
- Run the script `./analyse.py`. This will generate illustrative plots taking advantage of the `plot` action of `pineappl` in a folder called `plots`.

N.B. Running madgraph, especially with high Monte Carlo precision, may require
quite some time, depending on the process. The numerical precision, as all of
the madgraph input, is controlled by modifying the files in the `TEMPLATE`
folder. This repository is meant for long-term reproducibility, not for
production, which had been handled by ERN once and for all.

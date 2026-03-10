# ewk_corrns_nnpdf41

This folder contains the (madgraph) pinecards needed to generate pineappl grids
accurate to NLO QCD LO EWK (with mixed corrections) together with some
additional scripts to streamline the process, the validation of the results,
the computaiton of EWK K-factors and the analyses of the EWK corrections.
These runcards have been tested with madgraph v6.7 pineappl v.0.8.7 and
pinefarm v0.4.1. Runcards are contained in folders called with the same
name as the relevant nnpdf4.1 data sets, which are placed in folders called
`<process_name>`. The user has to take the following actions.
- Go into the `<process_name>` folder and run
`./copy.sh`
- Run `pinefarm`
The commands above generate pineappl grids that incorporate EW and
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
production, which was handled by ERN once and for all.

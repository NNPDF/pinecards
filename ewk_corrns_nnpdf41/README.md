# ewk_corrns_nnpdf41

This folder contains the material to generate EWK K-factors using madgraph
(v6.7). All the madgraph input is contained in folders called with the same
name as the relevant nnpdf4.1 data sets, which are placed in folders called
`<process_name>_qcd_corrns` (QCD baseline) and `<process_name_ewk_corrns>` (EWK
corrections). The user has to take the following actions.
- Go into the `<process_name>_qcd_corrns` folder and run
`./copy.sh`
`./run.sh`
Individual data sets can be run with the `run.sh` script within each data set
folder.
- Repeat the same commands in the `<process_name>_ewk_corrns` folder.
- Run the script `./compute_and_clean`. This will generate a folder, called
`K-factors` that is going to contain the relevant results. These can be
finally pasted in the relevant theories.

N.B. Running madgraph, especially with high Monte Carlo precision, may require
quite some time, depending on the process. The numerical precision, as all of
the madgraph input, is controlled by modifying the files in the `TEMPLATE`
folder. Tjis repository is meant for long-term reproducibility, not for
production, which had been handled by ERN once and for all.

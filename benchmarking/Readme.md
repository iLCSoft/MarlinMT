
## MarlinMT benchmarking

Files in this directory:

- *cpu_crunching.xml*: The XML steering file used for benchmarking MarlinMT. Options:
   - `--constant.TriggerUnpacking=[true;false]`: Whether to trigger the event decoding in the worker thread
   - `--global.Concurrency=[N]`: The number of worker threads to use
   - `--datasource.LazyUnpack=[true;false]`: Whether to forward the event decoding to a worker thread
   - `--CPUCrunch.CrunchTime=[N]`: The CPU crunching time within each worker (unit ms)
   - `--CPUCrunch.CrunchSigma=[N]`: A gaussian random value added to the crunch time (unit ms)
- *run-benchmarking*: a bash script running MarlinMT many times with different settings. The goal is to extract scaling performance curves. Use `./run-benchmarking --help` to see the various options
- *PlotScaling.C*: a ROOT macro for parsing the output of the `run-benchmarking` script and plotting scaling curves, nicely formatted :-)
- *run-all-benchmarks*: an example of running scenarios running multiple times `run-benchmarking` with different settings. Note that the current content of this may takes hours to run (run on a batch node at DESY in my case).

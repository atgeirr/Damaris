float* data;
int iteration;
data = DC_alloc("my group/temperature",iteration);
...
int err = DC_commit("my group/temperature",iteration);

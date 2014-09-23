float* data;
int err;
err = damaris_alloc("my group/temperature",&data);
...
// done writing data for this iteration
err = damaris_commit("my group/temperature");
...
// done accessing data for this iteration
err = damaris_clear("my group/temperature");
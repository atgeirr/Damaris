float* data[4];
...
for(i=0;i<4;i++)
  int err = DC_write_block("my group/temperature",i,data[i]);

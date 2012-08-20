int w_in;
int err = DC_parameter_get("w",&w_in,sizeof(int));

int w_out = 4;
int err = DC_parameter_set("w",&w_out,sizeof(int));
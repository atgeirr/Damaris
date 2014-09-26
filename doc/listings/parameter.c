int w_in;
int err = damaris_parameter_get("w",&w_in,sizeof(int));

int w_out = 4;
int err = damaris_parameter_set("w",&w_out,sizeof(int));
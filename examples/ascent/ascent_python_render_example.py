###############################################################################
# Copyright (c) Lawrence Livermore National Security, LLC and other Ascent
# Project developers. See top-level LICENSE AND COPYRIGHT files for dates and
# other details. No copyright assignment is required to contribute to Ascent.
###############################################################################


"""
 file: ascent_python_render_example.py

 description:
   Demonstrates using ascent to render a pseudocolor plot.
   
# these are for Ascent and Conduit
export PYTHONPATH=/home/jbowden/local/python-modules:$PYTHONPATH

"""

import conduit
import conduit.blueprint
import ascent
import numpy as np

# print details about ascent
print(ascent.about())

"""
import yaml #pip install --user pyyaml
import pandas as pd
import matplotlib.pyplot as plt

session = []
with open(r'ascent_session.yaml') as file:
  session = yaml.load(file)
"""

# open ascent
a = ascent.Ascent()
a.open()


# create example mesh using conduit blueprint
mesh_data = conduit.Node()
"""
conduit.blueprint.mesh.examples.braid("hexs",
                                      11,
                                      11,
                                      11,
                                      mesh_data)
"""

# Domain size:
# domain_V = 16
domain_W = 16
domain_H = 9  # block this into 3
domain_D = 16 # block this into 4

# Block sub-domain sizes:
# bloc_V   = 16
bloc_W   = 16  // 4
bloc_H   = 9   // 1
bloc_D   = 16  // 4 

bloc_tup        = (bloc_D, bloc_H, bloc_W)
domain_tup      = (domain_D, domain_H, domain_W)

"""
mesh_data["state/time"].set_external(&m_time);
mesh_data["state/cycle"].set_external(&m_cycle);
mesh_data["state/domain_id"] = myRank;
"""
from functools import reduce
from operator import mul
total_elems  = reduce(mul,([v for v in domain_tup]))
# // create the coordinate set
mesh_data["coordsets/coords/type"] = "uniform";
mesh_data["coordsets/coords/dims/i"] = domain_W + 1
mesh_data["coordsets/coords/dims/j"] = domain_H + 1
mesh_data["coordsets/coords/dims/k"] = domain_D + 1

# // add origin and spacing to the coordset (optional)
mesh_data["coordsets/coords/origin/x"] = 0. # -domain_tup[0]//2;
mesh_data["coordsets/coords/origin/y"] = 0. # -domain_tup[1]//2;
mesh_data["coordsets/coords/origin/z"] = 0. #-domain_tup[2]//2;
# double distancePerStep = 20.0/(numPerDim-1);
mesh_data["coordsets/coords/spacing/dx"] = 1.0;
mesh_data["coordsets/coords/spacing/dy"] = 1.0;
mesh_data["coordsets/coords/spacing/dz"] = 1.0;


mesh_data["topologies/topo/type"] = "uniform";
# reference the coordinate set by name
mesh_data["topologies/topo/coordset"] = "coords";
    
    
field_nparray = np.arange(total_elems, dtype=np.float32)
# .reshape(domain_tup)


mesh_data["fields/alternating/association"] = "element"; # or "vertex"
mesh_data["fields/alternating/topology"] = "topo";
mesh_data["fields/alternating/values"].set_external(field_nparray);

"""
// coordinate system data
mesh_data["coordsets/coords/type"] = "explicit";
mesh_data["coordsets/coords/x"].set_external(m_x);
mesh_data["coordsets/coords/y"].set_external(m_y);
mesh_data["coordsets/coords/z"].set_external(m_z);

// topology data
mesh_data["topologies/mesh/type"] = "unstructured";
mesh_data["topologies/mesh/coordset"] = "coords";
mesh_data["topologies/mesh/elements/shape"] = "hexs";
mesh_data["topologies/mesh/elements/connectivity"].set_external(m_nodelist);

// one or more scalar fields
mesh_data["fields/p/type"]        = "scalar";
mesh_data["fields/p/topology"]    = "mesh";
mesh_data["fields/p/association"] = "element";
mesh_data["fields/p/values"].set_external(m_p);
"""


# publish mesh to ascent
a.publish(mesh_data)

# declare a scene to render the dataset
scenes  = conduit.Node()

if True:
    scenes["s1/plots/p1/type"] = "pseudocolor"
    scenes["s1/plots/p1/field"] = "alternating"
else:
    scenes["s1/plots/p1/type"] = "mesh"
# scenes["s1/plots/p1/min_value"] = -0.5
# scenes["s1/plots/p1/max_value"] = 0.5
scenes["s1/image_prefix"] = "out_ascent_render_3d_"

scenes["s1/renders/r1/image_width"]  = 512
scenes["s1/renders/r1/image_height"] = 512
# Set the output file name (ascent will add ".png")
scenes["s1/renders/r1/image_prefix"] = "out_ascent_render2_3d_"



vec3 = np.array([1.0,1.0,1.0], dtype=np.float32)
vec3[0] = 1.
vec3[1] = 1.
vec3[2] = 1.
scenes["s1/renders/r1/camera/look_at"].set(vec3);  # deep copied

# position interacts with near_plane and far_plane by clipping
# data outside the range
vec3[0] = -domain_tup[0]
vec3[1] = -domain_tup[1]
vec3[2] = -domain_tup[2]
scenes["s1/renders/r1/camera/position"].set(vec3);
vec3[0] = 0.
vec3[1] = 0.
vec3[2] = 1.
scenes["s1/renders/r1/camera/up"].set(vec3);
scenes["s1/renders/r1/camera/fov"] = 80.;
scenes["s1/renders/r1/camera/xpan"] = 0.0;
scenes["s1/renders/r1/camera/ypan"] = 0.0;
scenes["s1/renders/r1/camera/azimuth"] = 0.0;
scenes["s1/renders/r1/camera/elevation"] = -10.0;
scenes["s1/renders/r1/camera/zoom"] = 1.5;
scenes["s1/renders/r1/camera/near_plane"] = 0.1;
scenes["s1/renders/r1/camera/far_plane"] = 100.1;



# setup actions to
actions = conduit.Node()
add_act =actions.append()
add_act["action"] = "add_scenes"
add_act["scenes"] = scenes

# execute
a.execute(actions)

# close alpine
a.close()





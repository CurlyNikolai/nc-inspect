import netCDF4 as nc
import numpy as np

# Create a new NetCDF file
with nc.Dataset('test.nc', 'w', format='NETCDF4') as rootgrp:
    # Define dimensions at root level
    rootgrp.createDimension('x', 10)
    rootgrp.createDimension('y', 20)
    rootgrp.createDimension('z', 5)
    rootgrp.createDimension('v', 15)
    rootgrp.createDimension('t', 100)
    rootgrp.createDimension('s', 100)

    # 2D variable with NaNs
    var_2d = rootgrp.createVariable('var_2d_with_nans', 'f4', ('x', 'y'))
    data_2d = np.random.rand(10, 20).astype(np.float32)
    data_2d[5, 10] = np.nan
    data_2d[2, 15] = np.nan
    var_2d[:] = data_2d

    # Sine data with time dependency (frequency changes over time) - now 2D with x dimension
    sine_data = rootgrp.createVariable('sine_data', 'f4', ('s', 't'))
    time = np.linspace(1, 5, 100)
    x_vals = np.linspace(0, 1.0, 100)  # 0 to 9
    sine_values = np.zeros((100, 100), dtype=np.float32)
    for i, t in enumerate(time):
        sine_values[:,i] = np.sin(2 * np.pi * x_vals * t)
    sine_data[:] = sine_values

    # Create first group
    group1 = rootgrp.createGroup('group1')
    group1.createDimension('a', 8)
    group1.createDimension('b', 12)

    # 3D variable in group1
    var_3d = group1.createVariable('var_3d', 'f4', ('x', 'y', 'a'))
    var_3d[:] = np.random.rand(10, 20, 8).astype(np.float32)

    # Create nested group in group1
    subgroup1 = group1.createGroup('subgroup1')
    subgroup1.createDimension('c', 6)

    # 4D variable in subgroup1
    var_4d = subgroup1.createVariable('var_4d', 'f4', ('x', 'y', 'z', 'c'))
    var_4d[:] = np.random.rand(10, 20, 5, 6).astype(np.float32)

    # Create second group
    group2 = rootgrp.createGroup('group2')
    group2.createDimension('d', 7)
    group2.createDimension('e', 9)

    # Another 1D variable in group2
    var_1d_g2 = group2.createVariable('var_1d_g2', 'f4', ('d',))
    var_1d_g2[:] = np.random.rand(7).astype(np.float32)

    # 2D variable in group2
    var_2d_g2 = group2.createVariable('var_2d_g2', 'f4', ('d', 'e'))
    var_2d_g2[:] = np.random.rand(7, 9).astype(np.float32)

print("Test NetCDF file 'test.nc' created successfully!")
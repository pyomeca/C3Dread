"""
Test for file IO
"""
from pathlib import Path

import numpy as np
import pytest

import ezc3d

def test_create_c3d():
    c3d = ezc3d.c3d()
    
    # Test the header 
    assert c3d['header']['points']['size'] == 0
    assert c3d['header']['points']['frame_rate'] == 0.0
    assert c3d['header']['points']['first_frame'] == 0
    assert c3d['header']['points']['last_frame'] == 0
    
    assert c3d['header']['analogs']['size'] == 0
    assert c3d['header']['analogs']['frame_rate'] == 0.0
    assert c3d['header']['analogs']['first_frame'] == 0
    assert c3d['header']['analogs']['last_frame'] == 0
    
    assert c3d['header']['events']['size'] == 18
    assert c3d['header']['events']['events_time'] == (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
    assert c3d['header']['events']['events_label'] == ('', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '')
    
    # Test the parameters
    assert c3d['parameters']['POINT']['USED']['value'][0] == 0
    assert c3d['parameters']['POINT']['SCALE']['value'][0] == -1
    assert c3d['parameters']['POINT']['RATE']['value'][0] == 0.0
    assert c3d['parameters']['POINT']['FRAMES']['value'][0] == 0
    assert len(c3d['parameters']['POINT']['LABELS']['value']) == 0
    assert len(c3d['parameters']['POINT']['DESCRIPTIONS']['value']) == 0
    assert len(c3d['parameters']['POINT']['UNITS']['value']) == 0
    
    assert c3d['parameters']['ANALOG']['USED']['value'][0] == 0
    assert len(c3d['parameters']['ANALOG']['LABELS']['value']) == 0
    assert len(c3d['parameters']['ANALOG']['DESCRIPTIONS']['value']) == 0
    assert c3d['parameters']['ANALOG']['GEN_SCALE']['value'][0] == 1
    assert len(c3d['parameters']['ANALOG']['SCALE']['value']) == 0
    assert len(c3d['parameters']['ANALOG']['OFFSET']['value']) == 0
    assert len(c3d['parameters']['ANALOG']['UNITS']['value']) == 0
    assert c3d['parameters']['ANALOG']['RATE']['value'][0] == 0.0
    assert len(c3d['parameters']['ANALOG']['FORMAT']['value']) == 0
    assert len(c3d['parameters']['ANALOG']['BITS']['value']) == 0
    
    assert c3d['parameters']['FORCE_PLATFORM']['USED']['value'][0] == 0
    assert len(c3d['parameters']['FORCE_PLATFORM']['TYPE']['value']) == 0
    assert c3d['parameters']['FORCE_PLATFORM']['ZERO']['value'] == (1, 0)
    assert len(c3d['parameters']['FORCE_PLATFORM']['CORNERS']['value']) == 0
    assert len(c3d['parameters']['FORCE_PLATFORM']['ORIGIN']['value']) == 0
    assert len(c3d['parameters']['FORCE_PLATFORM']['CHANNEL']['value']) == 0
    assert len(c3d['parameters']['FORCE_PLATFORM']['CAL_MATRIX']['value']) == 0
    
    # Test the data
    assert c3d['data']['points'].shape == (4, 0, 0)
    assert c3d['data']['analogs'].shape == (1, 0, 0)
    
    
def test_create_and_read_c3d():
    # Load an empty c3d structure
    c3d = ezc3d.c3d()

    # Fill it with random data
    point_names = ('point1', 'point2', 'point3', 'point4', 'point5')
    point_frame_rate = 100
    n_second = 2
    points = np.random.rand(3, len(point_names), point_frame_rate * n_second)

    analog_names = ('analog1', 'analog2', 'analog3', 'analog4', 'analog5', 'analog6')
    analog_frame_rate = 1000
    analogs = np.random.rand(1, len(analog_names), analog_frame_rate * n_second)

    c3d['parameters']['POINT']['RATE']['value'] = [100]
    c3d['parameters']['POINT']['LABELS']['value'] = point_names
    c3d['data']['points'] = points

    c3d['parameters']['ANALOG']['RATE']['value'] = [1000]
    c3d['parameters']['ANALOG']['LABELS']['value'] = analog_names
    c3d['data']['analogs'] = analogs

    # Add a custom parameter to the POINT group
    point_new_param = ("POINT", "newPointParam", (1.0, 2.0, 3.0))
    c3d.add_parameter(point_new_param[0], point_new_param[1], point_new_param[2])

    # Add a custom parameter a new group
    new_group_param = ("NewGroup", "newGroupParam", ["MyParam1", "MyParam2"])
    c3d.add_parameter(new_group_param[0], new_group_param[1], new_group_param[2])

    # Write and read back the data
    c3d.write("temporary.c3d")
    c3d_to_compare = ezc3d.c3d("temporary.c3d")

    # Test the header 
    assert c3d_to_compare['header']['points']['size'] == len(point_names)
    assert c3d_to_compare['header']['points']['frame_rate'] == point_frame_rate
    assert c3d_to_compare['header']['points']['first_frame'] == 0
    assert c3d_to_compare['header']['points']['last_frame'] == point_frame_rate * n_second - 1
    
    assert c3d_to_compare['header']['analogs']['size'] == len(analog_names)
    assert c3d_to_compare['header']['analogs']['frame_rate'] == analog_frame_rate
    assert c3d_to_compare['header']['analogs']['first_frame'] == 0
    assert c3d_to_compare['header']['analogs']['last_frame'] == analog_frame_rate * n_second - analog_frame_rate / point_frame_rate
    
    assert c3d_to_compare['header']['events']['size'] == 18
    assert c3d_to_compare['header']['events']['events_time'] == (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
    assert c3d_to_compare['header']['events']['events_label'] == ('', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '')
    
    # Test the parameters
    assert c3d_to_compare['parameters']['POINT']['USED']['value'][0] == len(point_names)
    assert c3d_to_compare['parameters']['POINT']['SCALE']['value'][0] == -1.0
    assert c3d_to_compare['parameters']['POINT']['RATE']['value'][0] == point_frame_rate
    assert c3d_to_compare['parameters']['POINT']['FRAMES']['value'][0] == point_frame_rate * n_second
    assert c3d_to_compare['parameters']['POINT']['LABELS']['value'] == list(point_names)
    assert c3d_to_compare['parameters']['POINT']['DESCRIPTIONS']['value'] == ["" for i in point_names]
    assert len(c3d_to_compare['parameters']['POINT']['UNITS']['value']) == 0
    assert c3d_to_compare['parameters'][point_new_param[0].upper()][point_new_param[1].upper()]['value'] == point_new_param[2]
    
    assert c3d_to_compare['parameters']['ANALOG']['USED']['value'][0] == len(analog_names)
    assert c3d_to_compare['parameters']['ANALOG']['LABELS']['value'] == list(analog_names)
    assert c3d_to_compare['parameters']['ANALOG']['DESCRIPTIONS']['value'] == ["" for i in analog_names]
    assert c3d_to_compare['parameters']['ANALOG']['GEN_SCALE']['value'][0] == 1
    assert c3d_to_compare['parameters']['ANALOG']['SCALE']['value'] == tuple([1.0 for i in analog_names])
    assert c3d_to_compare['parameters']['ANALOG']['OFFSET']['value'] == tuple([0 for i in analog_names])
    assert c3d_to_compare['parameters']['ANALOG']['UNITS']['value'] == ["V" for i in analog_names]
    assert c3d_to_compare['parameters']['ANALOG']['RATE']['value'][0] == analog_frame_rate
    assert len(c3d_to_compare['parameters']['ANALOG']['FORMAT']['value']) == 0
    assert len(c3d_to_compare['parameters']['ANALOG']['BITS']['value']) == 0
    
    assert c3d_to_compare['parameters']['FORCE_PLATFORM']['USED']['value'][0] == 0
    assert len(c3d_to_compare['parameters']['FORCE_PLATFORM']['TYPE']['value']) == 0
    assert c3d_to_compare['parameters']['FORCE_PLATFORM']['ZERO']['value'] == (1, 0)
    assert len(c3d_to_compare['parameters']['FORCE_PLATFORM']['CORNERS']['value']) == 0
    assert len(c3d_to_compare['parameters']['FORCE_PLATFORM']['ORIGIN']['value']) == 0
    assert len(c3d_to_compare['parameters']['FORCE_PLATFORM']['CHANNEL']['value']) == 0
    assert len(c3d_to_compare['parameters']['FORCE_PLATFORM']['CAL_MATRIX']['value']) == 0
    
    assert c3d_to_compare['parameters'][new_group_param[0].upper()][new_group_param[1].upper()]['value'] == new_group_param[2]
    
    # Test the data
    assert c3d_to_compare['data']['points'].shape == (4, len(point_names), point_frame_rate * n_second)
    assert c3d_to_compare['data']['analogs'].shape == (1, len(analog_names), analog_frame_rate * n_second)

    # Compare the read c3d
    np.testing.assert_almost_equal(c3d_to_compare['data']['points'][0:3, :, :], points)
    np.testing.assert_almost_equal(c3d_to_compare['data']['analogs'], analogs)


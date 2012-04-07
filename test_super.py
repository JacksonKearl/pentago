#!/usr/bin/env python

from __future__ import division
from interface import *

def test_wins():
  try:
    engine.super_win_test(100000)
  except AssertionError,e:
    m = re.match('side (\d+), rside (\d+)',str(e))
    side = int(m.group(1))
    rside = int(m.group(2))
    q = asarray(side,uint64).reshape((1,)).view(int16)
    print 'side %d, quadrants %d %d %d %d\n%s'%(side,q[0],q[1],q[2],q[3],show_side(side))
    print 'rside %d\n%s'%(rside,show_side(rside))
    raise

def test_rmax():
  engine.super_rmax_test(100000)

def test_bool():
  engine.super_bool_test()

def test_group():
  engine.group_test()

def test_action():
  engine.action_test(100000)

def test_standardize():
  engine.superstandardize_test(10000)

def test_super_action():
  engine.super_action_test(10000)

if __name__=='__main__':
  test_super_action()
  test_standardize()
  test_action()
  test_group()
  test_wins()
  test_rmax()
  test_bool()
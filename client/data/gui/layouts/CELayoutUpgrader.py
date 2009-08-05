#!/usr/bin/env python

# CELayoutUpgrader: Upgrades CEGUI layout XML files to the Unified Coordinate System
# Copyright (C) 2006 Seth Yastrov <syastrov@gmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

import sys
from string import *
import codecs
from glob import glob

from xml.dom import minidom

try:
  import xml.dom.ext
except ImportError:
  print 'Error importing PyXML: You need to install PyXML.'
  print 'The default XML pretty-printing implementation is pretty ugly.'

class InvalidDocumentError(Exception): pass

class UDim:
  def __init__(self, scale, offset):
    self.scale, self.offset = scale, offset
  def toString(self):
    return "{%f,%f}" % (self.scale, self.offset)
  def exists(self):
    return not (self.scale == self.offset == 0.0)

class UVector2:
  def __init__(self, xs, xo, ys, yo):
    self.xs, self.xo, self.ys, self.yo = xs, xo, ys, yo
  def toString(self):
    return "{{%f,%f},{%f,%f}}" % (self.xs, self.xo, self.ys, self.yo)
  def exists(self):
    return not (self.xs == self.xo == self.ys == self.yo == 0.0)
  
class URect:
  def __init__(self, ls, lo, ts, to, rs, ro, bs, bo):
    self.ls, self.lo, self.ts, self.to, self.rs, self.ro, self.bs, self.bo =\
      ls, lo, ts, to, rs, ro, bs, bo
  def toString(self):
    return "{{%f,%f},{%f,%f},{%f,%f},{%f,%f}}" %\
      (self.ls, self.lo, self.ts, self.to, self.rs, self.ro, self.bs, self.bo)
  def exists(self):
    return not (self.ls == self.lo == self.ts == self.to == self.rs ==\
      self.ro == self.bs == self.bo == 0.0)

def splitCoords(coords):
  result = {}
  for coord in coords.split(' '):
    name, value = coord.split(':',1)
    result[name] = value
  return result

def upgrade(file):
  try:
    doc = minidom.parse(file);
  except Exception, detail:
    raise IOError, ("Failed to parse `%s': %s" % (file, str(detail)))
  
  global upgradeCount
  upgradeCount = 0
  
  if (doc.firstChild.nodeName != 'GUILayout'):
    raise InvalidDocumentError
  
  for window in doc.getElementsByTagName('Window'):
    # Collect all the old properties for each window
    coordProperties = ['RelativeMinSize', 'AbsoluteMinSize',\
      'RelativeMaxSize', 'AbsoluteMaxSize',\
      'Rect', 'RelativeRect', 'AbsoluteRect',\
      'Position', 'AbsolutePosition', 'RelativePosition',\
      'Size', 'RelativeSize', 'AbsoluteSize']
      
    properties = {}
    for child in window.getElementsByTagName('Property'):
      if (child in window.childNodes):
        properties[child.getAttribute('Name')] = child.getAttribute('Value')
      
    for prop in coordProperties:
      if (properties.has_key(prop)):
        properties[prop] = splitCoords(properties[prop])
    
    def getUDim(properties, name):
      scale, offset = 0.0, 0.0
      if (properties.has_key(name)):
        scale = float(properties[name])
      if (properties.has_key('Relative'+name)):
        scale = float(properties['Relative'+name])
      if (properties.has_key('Absolute'+name)):
        offset = float(properties[('Absolute'+name)])
      return UDim(scale, offset)
    
    def getUVector2(properties, name, c1, c2):
      xs, xo, ys, yo = 0.0, 0.0, 0.0, 0.0
      if (properties.has_key(name)):
        xs, ys = float(properties[name][c1]), float(properties[name][c2])
      if (properties.has_key('Relative'+name)):
        xs, ys = float(properties['Relative'+name][c1]), float(properties['Relative'+name][c2])
      if (properties.has_key('Absolute'+name)):
        xo, yo = float(properties[('Absolute'+name)][c1]), float(properties[('Absolute'+name)][c2])
      return UVector2(xs, xo, ys, yo)
      
    def getURect(properties, name):
      ls, lo, ts, to, rs, ro, bs, bo = 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
      if (properties.has_key(name)):
        ls, ts = float(properties[name]['l']), float(properties[name]['t'])
        rs, bs = float(properties[name]['r']), float(properties[name]['b'])
      if (properties.has_key('Relative'+name)):
        ls, ts = float(properties['Relative'+name]['l']), float(properties['Relative'+name]['t'])
        rs, bs = float(properties['Relative'+name]['r']), float(properties['Relative'+name]['b'])
      if (properties.has_key('Absolute'+name)):
        lo, to = float(properties['Absolute'+name]['l']), float(properties['Absolute'+name]['t'])
        ro, bo = float(properties['Absolute'+name]['r']), float(properties['Absolute'+name]['b'])
      return URect(ls, lo, ts, to, rs, ro, bs, bo)
    
    width = getUDim(properties, 'Width')
    height = getUDim(properties, 'Height')
    xposition = getUDim(properties, 'XPosition')
    yposition = getUDim(properties, 'YPosition')
    position = getUVector2(properties, 'Position', 'x', 'y')
    size = getUVector2(properties, 'Size', 'w', 'h')
    minSize = getUVector2(properties, 'MinSize', 'w', 'h')
    maxSize = getUVector2(properties, 'MaxSize', 'w', 'h')
    rect = getURect(properties, 'Rect')
    
    # Add new unified ones
    def addProperty(parent, name, value):
      global upgradeCount
      upgradeCount += 1
      
      node = doc.createElement('Property')
      node.setAttribute('Name', name)
      node.setAttribute('Value', value)
      window.insertBefore(node, parent.firstChild)
      return node
    
    if (width.exists()):
      addProperty(window, 'UnifiedWidth', width.toString())
    if (height.exists()):
      addProperty(window, 'UnifiedHeight', height.toString())
    if (xposition.exists()):
      addProperty(window, 'UnifiedXPosition', xposition.toString())
    if (yposition.exists()):
      addProperty(window, 'UnifiedYPosition', yposition.toString())
    
    if (position.exists()):
      addProperty(window, 'UnifiedPosition', position.toString())
    if (size.exists()):
      addProperty(window, 'UnifiedSize', size.toString())
    if (minSize.exists()):
      addProperty(window, 'UnifiedMinSize', minSize.toString())  
    if (maxSize.exists()):
      addProperty(window, 'UnifiedMaxSize', maxSize.toString())  
    
    if (rect.exists()):
      addProperty(window, 'UnifiedAreaRect', rect.toString())  
    
  # Remove old coord system nodes
  for child in doc.getElementsByTagName('Property'):
    if (child.getAttribute('Name') in coordProperties):
      child.parentNode.removeChild(child)
      child.unlink()
  
  f = codecs.open(file, 'w', 'utf-8')
  #doc.writexml(f, "", "    ")
  xml.dom.ext.PrettyPrint(doc, f, indent='\t')
  f.close()

def usage(scriptname):
  print 'Usage: %s FILES' % scriptname
  print 'FILES can be a glob-style pattern or a list of files.'

def main(argv):
  print 'CELayoutUpgrader: Upgrades CEGUI layout XML files to the Unified Coordinate System'
  print 'Copyright (C) 2006 Seth Yastrov <syastrov@gmail.com>\n'
  
  if (len(argv) < 2):
    usage(argv[0])
    sys.exit(2)
  
  global upgradeCount
  
  for arg in argv[1:]:
    for file in glob(arg):
      try:
        upgrade(file)
      except InvalidDocumentError:
        print "'%s' => Failed. Does not appear to be a valid GUILayout file." % file
        sys.exit(2)
      except IOError, detail:
        print "'%s' => %s" % (file, detail)
      else:
        print "'%s' => Upgraded (%d properties)" % (file, upgradeCount)

if __name__ == "__main__":
  main(sys.argv)


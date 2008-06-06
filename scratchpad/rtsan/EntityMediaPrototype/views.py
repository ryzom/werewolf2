import main_classes

class Sob3DView:
    def __init__(self, emd):
	self.name = "3D"
	self.emd = emd
    def update(self):
	print "3D View update!"
	self.emd.update()

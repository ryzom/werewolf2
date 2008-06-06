class SOB:
    def __init__(self):
	self.views = dict()
	self.containers = dict()
	self.controllers = dict()
    def addView(self, view):
	self.views[view.name] = view
    def getView(self, name):
	return self.views[name]
    def addContainer(self, container):
        self.containers[container.name] = container
    def getContainer(self, name):
        return self.containers[name]
    def addController(self, controller):
        self.controllers[controller.name] = controller
    def getController(self, name):
        return self.controllers[name]
    def update(self):
	for view in self.views.values():
            view.update()

class IDirtyPropagator:
    def __init__(self):
        self.dirty = True
        self.inputs = dict()
        self.outputs = dict()
    def __del__(self):
        self.breakAllConnections()
        del self.dirty
        del self.inputs
        del self.outpus
    def setDirty(self):
        self.dirty = True
        self.propagateDirtyState()
    def setClean(self):
        self.dirty = False
    def makeInputConnection(self, other):
        if(self == other):
            return
        self.inputs[other] = other
        other.outputs[self] = self
    def breakInputConnection(self, other):
        del self.inputs[other]
        del other.outputs[self]
    def breakOutputConnection(self, other):
        del self.outputs[other]
        del other.inputs[self]
    def breakAllConnections(self):
        for other in self.inputs:
            del other.outputs[self]
            del self.inputs[other]
        for other in self.outputs:
            del other.inputs[self]
            del self.outputs[other]
    def propagateDirtyState(self):
        for output in self.outputs:
            output.setDirty()

class IProperty(IDirtyPropagator):
    def __init__(self, name, value):
        IDirtyPropagator.__init__(self)
        self.name = name
        self.value = value

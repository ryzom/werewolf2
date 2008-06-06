import main_classes
import entity_media_data

class IControllableMedia(main_classes.IDirtyPropagator):
    def __init__(self):
        main_classes.IDirtyPropagator.__init__(self)
        self.controllers = dict()
    def addController(self, ctrl):
        self.controllers[ctrl.name] = ctrl
        self.makeInputConnection(ctrl)
        ctrl.init()
    def getController(self, name):
        return self.controllers[name]
    def updateControllers(self):
        for ctrl in self.controllers:
            ctrl.update()

class CAnimClip(IControllableMedia):
    def __init__(self, animClipData, emd):
        IControllableMedia.__init__(self)
        self.animClipData = animClipData
        self.name = animClipData.name
        self.emd = emd
    def update(self):
        if(self.dirty):
            self.updateControllers()
            self.setClean()

class CAnimGroup(IControllableMedia):
    def __init__(self, animGrpData, emd):
        IControllableMedia.__init__(self)
        self.animGroupData = animGrpData
        self.name = animGrpData.name
        self.emd = emd
    def update(self):
        if(self.dirty):
            self.updateControllers()
            self.setClean()

class CAnimLayer(IControllableMedia):
    def __init__(self, animLayerData, emd):
        IControllableMedia.__init__(self)
        self.animLayerData = animLayerData
        self.name = animLayerData.name
        self.emd = emd
        self.mesh = None
    def playAnimation(self, name):
        print "Play animation %s!" % name
    def update(self):
        if(self.dirty):
            self.updateControllers()
            self.setClean()

class CSkeleton(IControllableMedia):
    def __init__(self, skeletonData, emd):
        IControllableMedia.__init__(self)
        self.skeletonData = skeletonData
        self.name = skeletonData.name
        self.emd = emd
    def update(self):
        if(self.dirty):
            self.updateControllers()
            self.setClean()

class CAttachmentGroup(IControllableMedia):
    def __init__(self, attachmentGrpData, emd):
        IControllableMedia.__init__(self)
        self.attachmentGroupData = attachmentGrpData
        self.name = attachmentGrpData.name
        self.emd = emd
    def update(self):
        if(self.dirty):
            self.updateControllers()
            self.setClean()

class CAttachmentPoint(IControllableMedia):
    def __init__(self, attachmentPtData, emd):
        IControllableMedia.__init__(self)
        self.attachmentPointData = attachmentPtData
        self.name = attachmentPtData.name
        self.emd = emd
    def update(self):
        if(self.dirty):
            self.updateControllers()
            self.setClean()

class CMesh(IControllableMedia):
    def __init__(self, meshData, emd):
        IControllableMedia.__init__(self)
        self.meshData = meshData
        self.name = meshData.name
        self.emd = emd
    def morph(self, name, value):
        print "Morphing %s.%s to %f" % (self.name, name, value)
    def update(self):
        if(self.dirty):
            print "Updating mesh %s" % self.name
            self.updateControllers()
            self.setClean()


class CMeshGroup(IControllableMedia):
    def __init__(self, meshGrpData, emd):
        IControllableMedia.__init__(self)
        self.meshGrpData = meshGrpData
        self.name = meshGrpData.name
        self.emd = emd
        self.mesh = None
        self.visible = False
    def show(self):
        self.visible = True
    def hide(self):
        self.visible = False
    def update(self):
        if(self.visible):
            if(self.dirty):
                print "Updating mesh group %s" % self.name
                self.updateControllers()
                if(self.mesh):
                    mesh.update()
                self.setClean()

class CEntityMedia(IControllableMedia):
    def __init__(self, sob, emdata):
        IControllableMedia.__init__(self)
        self.sob = sob
        self.emdata = emdata
        self.name = emdata.name
        self.meshgroups = dict()
        self.animlayers = dict()
        self.skeletons = dict()
        self.parent = None
        self.initMeshGroups()
        self.initAnimLayers()
    def addMeshGroup(self, meshGrp):
        self.meshgroups[meshGrp.name] = meshGrp
        self.makeInputConnection(meshGrp)
    def addAnimLayer(self, animLayer):
        self.animlayers[animLayer.name] = animLayer
        self.makeInputConnection(animLayer)
    def initMeshGroups(self):
        for meshGrpData in self.emdata.meshgroups.values():
            self.addMeshGroup(CMeshGroup(meshGrpData, self))
    def initAnimLayers(self):
        for animLayerData in self.emdata.animlayers.values():
            self.addAnimLayer(CAnimLayer(animLayerData, self))
    def showMesh(self, name):
        mesh = self.meshgroups[name]
        if(mesh):
            mesh.show()
            self.setDirty()
    def hideMesh(self, name):
        mesh = self.meshgroups[name]
        if(mesh):
            mesh.hide()
            self.setDirty()
    def playAnimation(self, layerName, name):
        layer = self.animlayers[layerName]
        if(layer):
            layer.playAnimation(name)
            self.setDirty()
    def getSkeleton(self, name):
        skeleton = self.skeletons[name]
        if(skeleton == None):
            if(parent):
                return parent.getSkeleton(name)
        return skeleton
    def update(self):
        if(self.dirty):
            self.updateControllers()
            self.updateMeshes()
            self.updateAnimations()
            self.setClean()
    def updateMeshes(self):
        for mesh in self.meshgroups.values():
            mesh.update()
    def updateAnimations(self):
        for animations in self.animlayers.values():
            animations.update()

class CSequentialSelector:
    def __init__(self):
        self.next = 0
    def select(array):
        current = self.next
        if(++self.next == len(array)):
            self.next = 0
        return array[current]

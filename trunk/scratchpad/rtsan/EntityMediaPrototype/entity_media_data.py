class CEntityMediaTemplate:
    def __init__(self, name):
        self.name = name

class CEntityMediaData:
    def __init__(self, name, description, emt):
        self.name = name
        self.description = description
        self.emt = emt
        self.meshgroups = dict()
        self.animlayers = dict()
        self.skeletons = dict()
        self.attachments = dict()
    def addMeshGroup(self, meshGrp):
        self.meshgroups[meshGrp.name] = meshGrp
    def getMeshGroup(self, name):
        return self.meshgroups[name]
    def addAnimLayer(self, animLayer):
        self.animlayers[animLayer.name] = animLayer
    def getAnimLayer(self, name):
        return self.animlayers[name]
    def addSkeleton(self, skeleton):
        self.skeletons[skeleton.name] = skeleton
    def getSkeleton(self, name):
        return self.skeletons[name]
    def addAttachment(self, attachment):
        self.attachments[attachment.name] = attachment
    def getAttachment(self, name):
        return self.attachments[name]
    
class CMeshGroupData:
    def __init__(self, name):
        self.name = name
        self.meshes = dict()
    def addMesh(self, mesh):
        self.meshes[mesh.name] = mesh
    def getMesh(self, name):
        return self.meshes[name]

class CMeshData:
    def __init__(self, name, skeleton, mesh):
        self.name = name
        self.skeleton = skeleton
        self.mesh = mesh

class CAnimLayerData:
    def __init__(self, name):
        self.name = name
        self.animgroups = dict()
    def addAnimGroup(self, animGrp):
        self.animgroups[animGrp.name] = animGrp
    def getAnimGroup(self, name):
        return self.animgroups[name]

class CAnimGroupData:
    def __init__(self, name, wrapMode, cycles, selector):
        self.name = name
        self.wrapMode = wrapMode
        self.cycles = cycles
        self.selector = selector
        self.animclips = dict()
    def addAnimClip(self, animClip):
        self.animclips[animClip.name] = animClip
    def getAnimClip(self, name):
        return self.animclips[name]

class CAnimClipData:
    def __init__(self, name, priority):
        self.name = name
        self.priority = priority

class CSkeletonData:
    def __init__(self, name, skeleton):
        self.name = name
        self.skeleton = skeleton

class CAttachmentGroupData:
    def __init__(self, name, capacity):
        self.name = name
        self.capacity = capacity
        self.attachments = dict()
    def addAttachment(self, attachment):
        self.attachments[attachment.name] = attachment
    def getAttachment(self, name):
        return self.attachments[name]

class CAttachmentPointData:
    def __init__(self, name):
        self.name = name

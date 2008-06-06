import main_classes
import entity_media_data
import data_containers
import views
import entity_media

char = main_classes.SOB()
char.addContainer(data_containers.CharacterData())
bipedEMT = entity_media_data.CEntityMediaTemplate("biped")
charEMD = entity_media_data.CEntityMediaData("char", "character media", bipedEMT)
charEMD.addMeshGroup(entity_media_data.CMeshGroupData("body"))
charEMD.addMeshGroup(entity_media_data.CMeshGroupData("lLeg"))
charEMD.addMeshGroup(entity_media_data.CMeshGroupData("rLeg"))
charEMD.addSkeleton(entity_media_data.CSkeletonData("body", "charBody.skel"))
charEMD.addSkeleton(entity_media_data.CSkeletonData("cape", "charCape.skel"))
charEMD.addAnimLayer(entity_media_data.CAnimLayerData("motion"))
charEMD.addAnimLayer(entity_media_data.CAnimLayerData("action"))
charEMD.getMeshGroup("body").addMesh(entity_media_data.CMeshData("default", "body", "charBodyDefault.shape"))
charEMD.getMeshGroup("lLeg").addMesh(entity_media_data.CMeshData("default", "body", "charLLegDefault.shape"))
charEMD.getMeshGroup("rLeg").addMesh(entity_media_data.CMeshData("default", "body", "charRLegDefault.shape"))
charEMD.getAnimLayer("motion").addAnimGroup(entity_media_data.CAnimGroupData("run", "repeat", 0, "sequential"))
charEMD.getAnimLayer("motion").addAnimGroup(entity_media_data.CAnimGroupData("walk", "repeat", 0, "sequential"))
charEMD.getAnimLayer("motion").addAnimGroup(entity_media_data.CAnimGroupData("idle", "repeat", 0, "sequential"))
charEMD.getAnimLayer("action").addAnimGroup(entity_media_data.CAnimGroupData("idle", "repeat", 0, "sequential"))
charEMD.getAnimLayer("action").addAnimGroup(entity_media_data.CAnimGroupData("wave", "repeat", 0, "sequential"))
char.addView(views.Sob3DView(entity_media.CEntityMedia(char, charEMD)))
view = char.getView("3D")
emd = view.emd

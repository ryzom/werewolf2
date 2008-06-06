import main_classes

class IDataContainer:
    def __init__(self):
        self.properties = dict()
    def addProperty(self, prop):
        self.properties[prop.name] = prop
    def getProperty(self, name):
        return self.properties[name]

class CharacterData(IDataContainer):
    def __init__(self):
        IDataContainer.__init__(self)
        self.name = "Character"
        self.fat = main_classes.IProperty("fat",10)
        self.addProperty(self.fat)
        self.age = main_classes.IProperty("age",22)
        self.addProperty(self.age)

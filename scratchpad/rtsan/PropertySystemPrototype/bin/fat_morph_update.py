def doIt(c):
    char = c.obj.emd.owner.getDataContainer('Character')
    if(char):
        c.obj.morph('fat', (char.fat-10)*2)


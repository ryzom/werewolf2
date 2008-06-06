def doIt(c):
    char = c.obj.emd.owner.getDataContainer('Character')
    if(char):           
        c.makeInputConnection(char.fatProperty)


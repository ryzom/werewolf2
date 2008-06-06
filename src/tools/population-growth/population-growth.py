#!/usr/bin/env python
#Boa:PyApp:main

modules ={}

def main():
    #history start date (how many years back are we going to look?)
    historystart=int(-15000)
    
    #starting population for society
    p=2000
    
    #use this if you want to have different population growth rates over time
    #first number is year the new growth rate starts, the second is the new
    #growth rate.
    pgrowthdict = {-12000:.0009, -9800:.00075, -6000:.0008, -200:.0009}    
    
    #average growth rate of population
    pgrowth=.00075
    
    #events that occur in time (war, prosperity, etc)
    #that affect population sizes
    pevents = {-12000:.80, -11000:-.75, -10000:-.95, -6000:-.01, -2600:.25, -2300:.10, -1480:-.25}
    
       
    print "Population Growth for Nations"
    print "===================================="
    print " "
    print "History Starts: " + str(historystart)
    print "Starting Population: " + str(p)
    print "Avg. Population Growth: " + str(pgrowth)
    print "Population Events & Severity: " + str(pevents)
    print " "
    print " "
    
    #numevents = int(raw_input("Please enter the number of population rate changes: "))
    
    print ""
    
    #for x in range(numevents):
    # 	pratechange = int(raw_input("Population Growth Rate Change Date: "))
    #    pnewrate = int(raw_input("New Population Growth Rate 1-100 (100=population doubles each year): "))
    #    pgrowthdict[pratechange] = pnewrate
    #    print pgrowthdict
	
    
    
    print "Rate: " + str(p)
    for i in range(historystart, 1, 1):
        if pgrowthdict.has_key(i):
            pgrowth=pgrowthdict[i]
            print "Changed rate to: " + str(p)
        else:
            pgrowth = pgrowth
        if pevents.has_key(i):  #this is an event year if true
            print "*********** Event Year Below ************"
            print "previous year: " + str(i-1) + "             Population: " + str(int(p))
            p = (p*pevents[i]) + p  #compute population based on event
            print "event year:    " + str(i) + "             Population: " + str(int(p))
            print
        else:
            p = (p*pgrowth) + p     #compute population normally
            #print "year: " + str(i) + "             Population: " + str(p)
    print "*********** Ending Population ************"
    print "year: " + str(i) + "             Population: " + str(int(p))
        
        

if __name__ == '__main__':
    main()

import csv

def runTest():
	test = ww.CEventTest()
	test.setDuration(6)
	test.setMessageSize(32)
	test.setMessageFrequency(2)
	test.setFps(1000)
	test.setNumBroadcasters(100)
	test.setNumHandlers(100)
	test.setDebug(False)
	test.run()
	print("FPS: " + str(test.getResultFps()))
	print("Frame time: " + str(test.getResultFrameTime()))
	print("Message frame time: " + str(test.getResultMessageFrameTime()))
	print("Total message time: " + str(test.getResultTotalMessageTime()))
	return test

def runSimpleTest(d, s):
	test = ww.CEventTest()
	test.setDuration(d)
	test.setMessageSize(s)
	test.setMessageFrequency(200);
	test.setFps(500);
	test.setNumBroadcasters(10);
	test.setNumHandlers(10);
	test.setDebug(True)
	test.run()

def runMessageSizeTest():
	outfile=open("messageSizeTest.csv", "w")
	wr=csv.writer(outfile, dialect="excel", delimiter=";", lineterminator="\n")
	wr.writerow(["Message size", "FPS", "Message delay"]) 
	test = ww.CEventTest()
	test.setMessageFrequency(60)
	test.setFps(1000)
	test.setNumBroadcasters(10)
	test.setNumPreListeners(10)
	test.setNumFilters(0)
	test.setNumHandlers(10)
	test.setNumPostListeners(10)
	test.setDuration(2)
	test.setDebug(False)
	for msgSize in range(16, 4001, 16):
		print "Running test with message size " + str(msgSize) + " bytes"
		test.setMessageSize(msgSize)
		test.run()
		wr.writerow([msgSize, test.getResultFps(), test.getResultMessageDelay()])
	del test
	outfile.close()
	del wr
	del outfile

def runDefault():
	outfile=open("defaultTest.csv", "w")
	wr=csv.writer(outfile, dialect="excel", delimiter=";", lineterminator="\n")
	wr.writerow(["Test #", "FPS", "Message delay"]) 
	test = ww.CEventTest()
	test.setMessageFrequency(60)
	test.setFps(1000)
	test.setNumBroadcasters(10)
	test.setNumPreListeners(10)
	test.setNumFilters(0)
	test.setNumHandlers(10)
	test.setNumPostListeners(10)
	test.setDuration(2)
	test.setDebug(False)
	test.setMessageSize(32)
	for i in range(1, 200, 1):
		print "Running test #" + str(i)
		test.run()
		wr.writerow([i, test.getResultFps(), test.getResultMessageDelay()])
	del test
	outfile.close()
	del wr
	del outfile

def runNumBroadcastersTest():
	outfile=open("numBroadcastersTest.csv", "w")
	wr=csv.writer(outfile, dialect="excel", delimiter=";", lineterminator="\n")
	wr.writerow(["Number of broadcasters", "FPS", "Message delay", "Message time", "Total message count", "Total message time", "Message frame time", "Number of message frames"]) 
	test = ww.CEventTest()
	test.setMessageFrequency(2)
	test.setFps(1000)
	test.setNumPreListeners(10)
	test.setNumFilters(0)
	test.setNumHandlers(100)
	test.setNumPostListeners(10)
	test.setDuration(4)
	test.setDebug(False)
	test.setMessageSize(32)
	for numBroadcasters in range(0, 1000, 5):
		print "Running test with " + str(numBroadcasters) + " broadcasters"
		test.setNumBroadcasters(numBroadcasters)
		test.run()
		wr.writerow([numBroadcasters, test.getResultFps(), test.getResultMessageDelay(), test.getResultMessageTime(), test.getResultTotalMessageCount(), test.getResultTotalMessageTime(), test.getResultMessageFrameTime(), test.getResultNumMessageFrames()])
	del test
	outfile.close()
	del wr
	del outfile



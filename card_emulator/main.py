import serial
import os

LINE_ENDING = chr(13)+chr(10)
TERMINATOR = chr(62)
MEMORY_FILE =  "image.bin"
PORT_IN = "/dev/ttyS10" # change this to your COM port

##
#
def createDataFile():
    global MEMORY_FILE
    
    if not os.path.isfile(MEMORY_FILE):
        file = open( MEMORY_FILE, "wb" )
        print("Creating memory file ...")
        for i in range(0,524288):
            file.write(b'0')
            
        file.close() 
        print("Memory file created.")
        
    pass
    
## Converts a string into a list of its ascii codes.
#
def toAscii(text):
    data = []
    for char in text:
        data.append(ord(char)) 
    return data
   
## Converts a list of ascii codes into a string.
#   
def toString(data):
    res = ""
    for c in data:
        res = res + chr(c)
    return res
  
##
#  
def hex2bin():
    file1 = open("mem_hex.dat","rb")
    file = open("data.bin","wb+")
    
    i = 0
    while i < 524288:
        hex = file1.read(2)
        dec = int(hex,16)
        print(hex)
        print(dec)
        file.write(bytes([ord(chr(dec))]))
        i = i +1
        
    file1.close()
    file.close()

## Writes strings to the serial connection.
#
def writeStrings(s,lines):
    global LINE_ENDING, TERMINATOR
    for l in lines:
        s.write(str.encode( l + LINE_ENDING ))
    s.write(str.encode( TERMINATOR ))
  
## Writes data to the serial connection.
#  
def writeData(s,data):
    global LINE_ENDING, TERMINATOR
    s.write(data + str.encode( LINE_ENDING ) )
    s.write(str.encode( TERMINATOR ))

## Writes data to the memory file
#
def writeMemory(addr,data,blockSize=256):
    global MEMORY_FILE
    #createDataFile()
    file = open(MEMORY_FILE,"rb+")
    file.seek(addr*blockSize)
    file.write( bytes(data) )
    file.close()
  
## Reads data from the memory file.
#  
def readMemory(addr, blockSize=256):
    global MEMORY_FILE
    #createDataFile()
    file = open(MEMORY_FILE,"rb")
    file.seek(addr*blockSize)
    data = file.read(blockSize);
    file.close()
    return data

## Converts a addr(hex representation) to the int value.
#
def getAddr(cmd):
    parts = cmd.split(" ")
    addr = int(parts[1]+parts[2],16)
    return addr

## Parses a received commando.
#
def parseCommand(s,cmd):
    
    tcmd = toString(cmd[:-1])

    print( "command received: "+ tcmd[0:15] + " ..." )
    
    if tcmd == "model":
        writeStrings(s,[tcmd,"UltimateBox 1000"])
        
    elif tcmd == "version":
        writeStrings(s,[tcmd,"UltimateBox Rev 1000"])
        
    elif tcmd == "cd":
        writeStrings(s,[tcmd,"Found"])
    

    
    elif tcmd == "id6":
        writeStrings(s,[tcmd+"01D50001"])

    elif tcmd == "id5":
        writeStrings(s,[tcmd+"43706F79"])
        
    elif tcmd == "id4":
        writeStrings(s,[tcmd+"01D50001"])
        
    elif tcmd == "id3":
        writeStrings(s,[tcmd+""])
        
    elif tcmd == "id2":
        writeStrings(s,[tcmd+""])
        
    elif tcmd == "id1":
        writeStrings(s,[tcmd+""])
        
    elif tcmd == "id0":
        writeStrings(s,[tcmd+""])
        

    elif tcmd == "erase40":
        writeStrings(s,[tcmd,"OK"])
        
        
    elif tcmd[:5] == "bws40":
        addr = getAddr(tcmd[:11])
        writeMemory(
            addr,
            cmd[11:-1]
        )
        
    elif tcmd[:5] == "bread":
        addr = getAddr(tcmd)
        data = readMemory(addr)
        writeData(s,str.encode(tcmd+"@")+data)

    else:
        print( "unknown command: "+tcmd )
    
##
#
def main():

    global PORT_IN

    port = serial.Serial(
        PORT_IN,
        115200,
        timeout=0,
        parity=serial.PARITY_EVEN,
        rtscts=1
    )
    
    cmd = []
    i = 0
    while True:
#        i = i +1
#        if i > 5000000:
#            print("current data:" + str(len(cmd)) )
#            print( toString( cmd ))
        
        for c in port.read():
            cmd.append(c)
            
            if (c == 13 and cmd[:3] != toAscii("bws")):
                parseCommand(port, cmd)
                cmd = []
                i = 0
                
            elif (cmd[:3] == toAscii("bws") and len(cmd) == 268):
                parseCommand(port, cmd)
                cmd = []
                i = 0
            
    port.close()

createDataFile()
main()



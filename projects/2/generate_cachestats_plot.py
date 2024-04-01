#!/usr/bin/python

import sys
import getopt
import glob

l1missrate = 0
l2missrate = 0
l3missrate = 0

def getMissRate(reportRoot, reportName, cacheName):
  fileList = glob.glob(reportRoot + "/" + reportName)
  assert len(fileList) == 1
  fileName = fileList[0]

  f = open(fileName,"r")
  if f==0:
    return 0

  readHits = 0
  readMisses = 0
  writeHits = 0
  writeMisses = 0
  while True:
    l = f.readline()
    if l.find(cacheName + ":") != -1:
      cols = l.split(":")
      for col in cols:
        if col.find("readHits") != -1:
          readHits = int(col.split("=")[1])
        if col.find("readMisses") != -1:
          readMisses = int(col.split("=")[1])
        if col.find("writeHits") != -1:
          writeHits = int(col.split("=")[1])
        if col.find("writeMisses") != -1:
          writeMisses = int(col.split("=")[1])
      break
    if len(l)==0:
      break
  f.close
  if readHits + writeHits + readMisses + writeMisses > 0:
    return float(readMisses + writeMisses) / float(readHits + writeHits + readMisses + writeMisses)
  else:
    return 0

def main():
  try:
    opts, args = getopt.getopt(sys.argv[1:],"hi:o:",["inputDir=","outputFile="])
  except getopt.GetoptError:
    print ('generate_cachestats_plot.py -i <input directory> -o <output file>')
    sys.exit(1)
  for opt, arg in opts:
    if opt == '-h':
       print ('generate_cachestats_plot.py -i <input directory> -o <output file>')
       sys.exit()
    elif opt in ("-i", "--inputDir"):
       reportRoot = arg
    elif opt in ("-o", "--outputFile"):
       fileName = arg

  appNames = ["array", "linked-list"]
  confNames = ["wb-dl1-32k-l2-128k-64b", "wb-dl1-32k-l2-128k-8b", "wt-dl1-32k-l2-128k-64b"]
  cacheNames = ["DL1Cache", "L2Cache"]
  dataSizes = []
  for x in range(1000, 31000, 1000):
    dataSizes.append(x)

  f = open(fileName,"w")
  f.write("%10s " % "#Elements")
  for app in appNames:
    for conf in confNames:
      for cache in cacheNames:
        f.write("%15s " % (app + "." + conf + "." + cache))
  f.write('\n')
  for size in dataSizes:
    f.write("%10d " % size)
    for app in appNames:
      for conf in confNames:
        for cache in cacheNames:
          missRate = getMissRate(reportRoot, app + "." + str(size) + "." + conf + ".out", cache)
          f.write("%15.4f" % missRate)
    f.write('\n')
  f.close

if __name__ == "__main__":
  main()

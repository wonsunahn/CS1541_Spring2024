#!/usr/bin/python

import sys
import getopt
import glob

def getIPC(reportRoot, reportName):
  fileList = glob.glob(reportRoot + "/" + reportName)
  assert len(fileList) == 1
  fileName = fileList[0]

  f = open(fileName,"r")
  if f==0:
    return 0

  while True:
    l = f.readline()
    if l.find("IPC") != -1:
      cols = l.split(":")
      ipc = float(cols[1].replace("\n","").replace(" ", ""))
      break
    if len(l)==0:
      break
  f.close
  return ipc

def main():
  try:
    opts, args = getopt.getopt(sys.argv[1:],"hi:o:",["inputDir=","outputFile="])
  except getopt.GetoptError:
    print ('generate_perf_plot.py -i <input directory> -o <output file>')
    sys.exit(1)
  for opt, arg in opts:
    if opt == '-h':
       print ('generate_perf_plot.py -i <input directory> -o <output file>')
       sys.exit()
    elif opt in ("-i", "--inputDir"):
       reportRoot = arg
    elif opt in ("-o", "--outputFile"):
       fileName = arg

  appNames = ["array", "linked-list"]
  confNames = ["wb-dl1-32k-l2-128k-64b", "wb-dl1-32k-l2-128k-8b", "wt-dl1-32k-l2-128k-64b"]
  dataSizes = []
  for x in range(1000, 31000, 1000):
    dataSizes.append(x)

  f = open(fileName,"w")
  f.write("%10s " % "#Elements")
  for app in appNames:
    for conf in confNames:
      f.write("%15s " % (app + "." + conf))
  f.write('\n')
  for size in dataSizes:
    f.write("%10d " % size)
    for app in appNames:
      for conf in confNames:
        ipc = getIPC(reportRoot, app + "." + str(size) + "." + conf + ".out")
        f.write("%15.4f" % ipc)
    f.write('\n')
  f.close

if __name__ == "__main__":
  main()

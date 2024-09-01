import sys 
path = 'C:\\Users\\Admin\\Desktop\\MyOutput\\MyPrintPython.txt'
sys.stdout = open(path, 'w')
print('Hello, World')
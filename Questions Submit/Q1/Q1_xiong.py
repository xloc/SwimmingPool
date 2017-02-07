workhour=input('enter work hour:')
workrate=input('enter work rate:')
if workhour>40:
    pay=40*workrate+(workhour-40)*workrate*1.5
else:
    pay=workhour*workrate
print 'pay:',pay

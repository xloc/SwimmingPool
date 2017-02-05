work_hours=input('Enter Hours: ')
rate=input('Enter Rate: ')
if work_hours>40:
	salary=(work_hours-40)*rate*1.5+40*rate
else:
	salary=work_hours*rate 
print 'Pay:',salary

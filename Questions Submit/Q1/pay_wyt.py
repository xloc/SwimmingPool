Hours=float(raw_input('Plz enter the hours:'))
Rate=float(raw_input('Plz enter the rate:'))
if Hours<0:
	print 'Plz enter a number larger than 0'
elif 0<Hours<40:
    print 'Pay:', Hours*Rate
else:
    print 'Pay:', 40*Rate+(Hours-40)*1.5*Rate
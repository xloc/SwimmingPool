# author Cadmium
working_hours = float(raw_input('Enter Hours:'))
working_rate = float(raw_input('Enter Rate:'))

if working_hours <= 0:
    print 'No pay'
elif working_hours <= 40:
    money = working_hours * working_rate
    print 'Pay: %0.1f' % money
else:
    money = 40 * working_rate + (working_hours - 40) * working_rate
    print 'Pay: %0.1f' % money

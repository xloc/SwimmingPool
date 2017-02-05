hw=int(raw_input("Input your hourly wage:"))
t=int(raw_input("Input your working hours:"))
if t>40:
    w=(t-40)*hw*1.5+40*hw
    print "Your total wage is %d",w
else:
    t<=40
    w=hw*t
    print "Your total wage is %d",w


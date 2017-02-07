work_time=-1

while work_time<0:
    work_time=input("Enter Hours:")
    rate=input("Enter Rate:")

    if work_time<0:
        print 'HOW DARE YOU, MOTHER FUCKER! TRY AGAIN!'
    else:
        if work_time>40:
            salary=((work_time-40)*1.5+40)*rate
            print 'Pay:'+repr(salary)
        else:
            salary=work_time*rate
            print 'Pay:'+repr(salary)



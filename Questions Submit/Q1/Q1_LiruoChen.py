#!/usr/bin/env python3
# -*- coding: utf-8 -*-
hour = input('please enter the work time:')
rate = input('please enter the work salary per hour:')
def payfunction(hour, rate):
    print('hours:', hour)
	print('salary rate:', rate)
    pay=0
    if int(hour) <= 40:
        pay=int(hour)*int(rate)
	    print('pay:', pay)
    else:
        pay=40*int(rate) + (int(hour)-40)*int(rate)*1.5
	    print('pay:', pay)




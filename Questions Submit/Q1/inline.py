print (lambda mph, t: t*mph + (0.5*(t-40)*mph if t > 40 else 0))(
    *map(input, ['Money per hour: ', 'Work time: ']))
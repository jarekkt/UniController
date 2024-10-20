



f = 50000
pps = 20
vel  = 10
acc  = 4
time = 5

print('Base ----')
print(vel)
print(pps)
print(acc)
print(time)
print(vel*time + 0.5*acc*time*time)
print(vel + acc *time)


fvel = vel * pps / f
facc = acc * pps / (f*f)


accum = 0
for ii in range(0,int(time*f)):
   fvel = fvel + facc
   accum = accum +fvel

print('Run up 0----')
print( '0- dist={}'.format(accum/pps))
print('0- vel={}'.format(fvel*f/pps))
print('0- acc={}'.format(facc*f*f/pps))

jerk = 4* acc / time
fjcc = jerk * pps / (f*f*f)
fvel = vel * pps / f

print('Run up 1----')
accum = 0
facc = 0
for ii in range(0,int(time*f/2)):
   facc = facc + fjcc
   fvel = fvel + facc
   accum = accum + fvel

print( '1- dist={}'.format(accum/pps))
print('1- vel={}'.format(fvel*f/pps))
print('1- acc={}'.format(facc*f*f/pps))

print('Run up 2----')
for ii in range(0,int(time*f/2)):
   facc = facc - fjcc
   fvel = fvel + facc
   accum = accum + fvel


print( '2- dist={}'.format(accum/pps))
print('2- vel={}'.format(fvel*f/pps))
print('2- acc={}'.format(facc*f*f/pps))

print('Run down 3----')

accum = 0
facc = 0
for ii in range(0,int(time*f/2)):
   facc = facc - fjcc
   fvel = fvel + facc
   accum = accum + fvel

print( '3- dist={}'.format(accum/pps))
print('3- vel={}'.format(fvel*f/pps))
print('3- acc={}'.format(facc*f*f/pps))

print('Run dn 4----')


for ii in range(0,int(time*f/2)):
   facc = facc + fjcc
   fvel = fvel + facc
   accum = accum + fvel

print( '4- dist={}'.format(accum/pps))
print('4- vel={}'.format(fvel*f/pps))
print('4- acc={}'.format(facc*f*f/pps))





f = 50000
time = 0.2

vf=0.0002
af=0
jf=7.9999999999999995e-11

acc = 0
for ii in range(0,int(time*f)):
   af =af +jf
   vf = vf + af
   acc = acc +vf

print(acc)
print(vf)
print(af)

print(vf *f)
print(af *f *f)



time = 0.045

vf=0.0042
af=7.9999999999999996e-07
jf=0

acc = 0
for ii in range(0,int(time*f)):
   af =af +jf
   vf = vf + af
   acc = acc +vf

print('----')
print(acc)
print(vf)
print(af)

print(vf *f)
print(af *f *f)

time = 0.2
vf=0.006
af=7.9999999999999996e-07
jf=-7.9999999999999995e-11

acc = 0
for ii in range(0,int(time*f)):
   af =af +jf
   vf = vf + af
   acc = acc +vf

print('----')
print(acc)
print(vf)
print(af)

print(vf *f)
print(af *f *f)

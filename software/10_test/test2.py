



f = 100000
pps = 31940/1000
time = 0.04

vf=0.0031939999999999998
af=0
jf=3.1939999999999999e-09

print('1A----')
print(vf *f/pps)
print(af *f *f/pps)
print(jf *f *f*f/pps)

acc = 0
for ii in range(0,int(time*f)):
   af =af +jf
   vf = vf + af
   acc = acc +vf

print('1B----')
print(acc)
print(vf)
print(af)

print('----')
print(vf *f/pps)
print(af *f *f/pps)
print(jf *f *f*f/pps)

time = 0.0965793803

vf=0.028746000000000001
af=1.2775999999999999e-05
jf=0

print('2A----')

print(vf *f/pps)
print(af *f *f/pps)
print(jf *f *f*f/pps)



acc = 0
for ii in range(0,int(time*f)):
   af =af +jf
   vf = vf + af
   acc = acc +vf

print('2B----')
print(acc)
print(vf)
print(af)
print('----')
print(vf *f/pps)
print(af *f *f/pps)
print(jf *f *f*f/pps)



print('3A----')
time = 0.04
vf=0.15213582080688476
af=1.2775999999999999e-05
jf=-3.1939999999999999e-09

print(vf *f/pps)
print(af *f *f/pps)
print(jf *f *f*f/pps)



acc = 0
for ii in range(0,int(time*f)):
   af =af +jf
   vf = vf + af
   acc = acc +vf

print('3B----')
print(acc)
print(vf)
print(af)
print('----')
print(vf *f/pps)
print(af *f *f/pps)
print(jf *f *f*f/pps)

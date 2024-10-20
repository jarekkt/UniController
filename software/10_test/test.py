import matplotlib.pyplot as plt
import array as arr
import math
import numpy as np



class motion_test:
    def __init__(self):
        self.safeZ = -7
        self.Xc = arr.array('f')
        self.Yc = arr.array('f')
        self.Zc = arr.array('f')
        self.N = arr.array('f')

    def path_moveTo(self,x,y,z,nozzle):
        self.Xc.append(x)
        self.Yc.append(y)
        self.Zc.append(z)
        self.N.append(nozzle)

    def path_plot(self,pltO):
        ax = pltO.axes(projection='3d')
        ax.plot3D(self.Xc, self.Yc, self.Zc, 'gray')

        Xs, Ys = np.mgrid[0:300:50, 0:300:50]
        Z1s = self.safeZ * np.ones_like(Xs)
        Z2s = -self.safeZ * np.ones_like(Xs)
        ax.plot_surface(Xs, Ys, Z1s, alpha=0.3, color='green')
        ax.plot_surface(Xs, Ys, Z2s, alpha=0.3, color='cyan')


    def trajectory(self):

        za = -15
        zb = -13

        # pick & place, one nozzle, symmetric
        
        self.path_moveTo(0, 0, self.safeZ, 1)
        self.path_moveTo(0, 0, za, 1)
        self.path_moveTo(0, 0, self.safeZ, 1)
        self.path_moveTo(100, 0, self.safeZ, 1)
        self.path_moveTo(100, 0, za, 1)
        self.path_moveTo(100, 0, self.safeZ, 1)
        self.path_moveTo(120, 0, self.safeZ, 1)
        self.path_moveTo(120, 0, za, 1)
        self.path_moveTo(120, 0, self.safeZ, 1)
        self.path_moveTo(124, 0, self.safeZ, 1)
        self.path_moveTo(124, 0, za, 1)
        self.path_moveTo(124, 0, self.safeZ, 1)
        self.path_moveTo(125, 0, self.safeZ, 1)
        self.path_moveTo(125, 0, za, 1)
        self.path_moveTo(125, 0, self.safeZ, 1)

        # pick & place, one nozzle, asymmetric
        
        self.path_moveTo(0, 50, self.safeZ, 1)
        self.path_moveTo(0, 50, zb, 1)
        self.path_moveTo(0, 50, self.safeZ, 1)
        self.path_moveTo(100, 50, self.safeZ, 1)
        self.path_moveTo(100, 50, za, 1)
        self.path_moveTo(100, 50, self.safeZ, 1)
        self.path_moveTo(120, 50, self.safeZ, 1)
        self.path_moveTo(120, 50, zb, 1)
        self.path_moveTo(120, 50, self.safeZ, 1)
        self.path_moveTo(124, 50, self.safeZ, 1)
        self.path_moveTo(124, 50, za, 1)
        self.path_moveTo(124, 50, self.safeZ, 1)
        self.path_moveTo(125, 50, self.safeZ, 1)
        self.path_moveTo(125, 50, zb, 1)
        self.path_moveTo(125, 50, self.safeZ, 1)

        #pick & place, dual nozzle, symmetric
        
        self.path_moveTo(0, 100, self.safeZ, 1)
        self.path_moveTo(0, 100, za, 1)
        self.path_moveTo(0, 100, self.safeZ, 1)
        self.path_moveTo(100, 100, -self.safeZ, 2)
        self.path_moveTo(100, 100, 15, 2)
        self.path_moveTo(100, 100, -self.safeZ, 2)
        self.path_moveTo(120, 100, self.safeZ, 1)
        self.path_moveTo(120, 100, za, 1)
        self.path_moveTo(120, 100, self.safeZ, 1)
        self.path_moveTo(124, 100, -self.safeZ, 2)
        self.path_moveTo(124, 100, 15, 2)
        self.path_moveTo(124, 100, -self.safeZ, 2)
        self.path_moveTo(125, 100, self.safeZ, 1)
        self.path_moveTo(125, 100, za, 1)
        self.path_moveTo(125, 100, self.safeZ, 1)

        # pick & place, dual nozzle, asymmetric
        
        self.path_moveTo(0, 150, self.safeZ, 1)
        self.path_moveTo(0, 150, za, 1)
        self.path_moveTo(0, 150, self.safeZ, 1)
        self.path_moveTo(100, 150, -self.safeZ, 2)
        self.path_moveTo(100, 150, 15, 2)
        self.path_moveTo(100, 150, -self.safeZ, 2)
        self.path_moveTo(120, 150, self.safeZ, 1)
        self.path_moveTo(120, 150, zb, 1)
        self.path_moveTo(120, 150, self.safeZ, 1)
        self.path_moveTo(124, 150, -self.safeZ, 2)
        self.path_moveTo(124, 150, 15, 2)
        self.path_moveTo(124, 150, -self.safeZ, 2)
        self.path_moveTo(125, 150, self.safeZ, 1)
        self.path_moveTo(125, 150, zb, 1)
        self.path_moveTo(125, 150, self.safeZ, 1)

        # move to push / pull feeder
        
        self.path_moveTo(200, 50, self.safeZ, 1)
        self.path_moveTo(220, 50, self.safeZ - 5, 1)
        self.path_moveTo(220, 50, self.safeZ, 1)
        self.path_moveTo(200, 80, self.safeZ, 1)
        self.path_moveTo(190, 100, self.safeZ, 1)
        self.path_moveTo(190, 120, self.safeZ, 1)
        self.path_moveTo(300, 120, za, 1)
        self.path_moveTo(300, 150, za, 1)
        self.path_moveTo(280, 150, za, 1)
        self.path_moveTo(279, 150, za, 1)
        self.path_moveTo(275, 150, za, 1)
        self.path_moveTo(275, 150, self.safeZ, 1)
        
        
class cut_rev_test:
    def __init__(self,tick_test):
        self.t_zcut = arr.array('f')
        self.t_zcut_time = arr.array('f')

        self.t_zcut.append(0.005 * self.m_dist)
        self.t_zcut.append(0.01 * self.m_dist)
        self.t_zcut.append(0.05 * self.m_dist)
        self.t_zcut.append(0.1 * self.m_dist)
        self.t_zcut.append(0.25 * self.m_dist)
        self.t_zcut.append(0.5 * self.m_dist)
        self.t_zcut.append(0.75 * self.m_dist)
        self.t_zcut.append(0.9 * self.m_dist)
        self.t_zcut.append(0.95 * self.m_dist)
        self.t_zcut.append(0.99 * self.m_dist)
        self.t_zcut.append(0.995 * self.m_dist)

    def calc_rev_check(self):
        for ii in range(0, len(self.t_zcut)):
            self.t_zcut_time.append(self.tick_test.calc_rev_time(self.t_zcut[ii]))

    def plot(self,plt):
        # Plot the data
        plt.plot(self.tick_test.t_time, self.tick_test.t_dist,label='distance')
        plt.plot(self.t_zcut_time,self.t_zcut,'ro',marker='x',label='zcut')




class motion_profile:
    def __init__(self, speed, accel, jerk, speed0):
        self.m_speed = speed
        self.m_speed0 = speed0
        self.m_accel = accel
        self.m_jerk = jerk

    def scale(self,coeff):
        self.m_speed = self.m_speed * coeff
        self.m_speed0 = self.m_speed0 * coeff
        self.m_accel = self.m_accel * coeff
        self.m_jerk = self.m_jerk * coeff


class motion_profile_caster:
        def __init__(self, Xp,Yp,Xe,Ye,mx,my,mp,coordinated):

            self.dX = Xe-Xp
            self.dY = dY
            self.mx = mx
            self.my = my
            self.mp = mp
            self.mp_x_cast = mp
            self.mp_y_cast = mp

            self.mp_x_cast.scale(math.cos(math.atan2(dY,dX)))
            self.mp_y_cast.scale(math.sin(math.atan2(dY, dX)))

class tick_calc:
    def __init__(self,dist,profile,tick_time,log=0,test_case=0):
        self.m_dist    = dist
        self.m_speed   = profile.m_speed
        self.m_speed_ini = profile.m_speed
        self.m_speed0  = profile.m_speed0
        self.m_accel_s = profile.m_accel
        self.m_jerk    = profile.m_jerk

        self.m_tick    = tick_time
        self.c_speed   = self.m_speed0
        self.c_accel   = 0
        self.c_jerk    = 0
        self.c_dist    = 0
        self.c_tick    = 0
        self.c_tick_total = 0
        self.c_idx     = 0
        self.log       = log
        self.test_case = test_case


        if self.test_case:
            self.test_set = '\t{}/*dist*/, {} /* speed0 */, {} /* speed */, {} /* accel */, {} /* jerk */'.format(self.m_dist, self.m_speed0, self.m_speed, self.m_accel_s,self.m_jerk)

        self.t_dist   = arr.array('f')
        self.t_speed  = arr.array('f')
        self.t_accel  = arr.array('f')
        self.t_jerk   = arr.array('f')
        self.t_time   = arr.array('f')

    def calc_phase(self):
        half_dist = self.m_dist / 2

        if True:
            # Calculate acceleration which would make ideal S curve
            coeff = [1/(self.m_jerk), 0, 2*self.m_speed0, -half_dist * self.m_jerk]
            roots = np.roots(coeff)
            a_scurve = roots[2].real

            # If requested acceleration is bigger then ideal S curve
            # then we have to limit it - it is too much for calculation distance

            if self.log:
                print('Acceleration our {} ideal S {} '.format(self.m_accel_s,a_scurve))

            if a_scurve < self.m_accel_s:
                old_acc = self.m_accel_s
                self.m_accel_s = a_scurve
                if self.log:
                    print('Changing acceleration from {} to {} (ideal s curve distance limit)'.format(old_acc,self.m_accel_s))

        # vs = v0 + a^2/j
        top_speed = self.m_speed0 + self.m_accel_s * self.m_accel_s/self.m_jerk

        if top_speed > self.m_speed:
            #Given acceleration makes too big speed - even for ideal S curve
            old_acc = self.m_accel_s
            old_speed = self.m_speed
            self.m_accel_s = math.sqrt((self.m_speed - self.m_speed0)*self.m_jerk)
            self.m_speed   = self.m_speed0 + self.m_accel_s * self.m_accel_s/self.m_jerk
            if self.log:
                print('Changing acceleration {}->{} and speed {}->{} (max speed limit)'.format(old_acc, self.m_accel_s,old_speed,self.m_speed ))



        self.T11 = self.m_accel_s / self.m_jerk
        self.T13 = self.T11
        self.T12 = (self.m_speed - self.m_speed0 - (self.m_accel_s * self.m_accel_s)/self.m_jerk)/self.m_accel_s
        if self.T12 < 0:
            self.T12 = 0


        self.s_t1 = (self.m_speed0 + (self.m_accel_s * self.m_accel_s)/(6*self.m_jerk))*(self.m_accel_s/self.m_jerk)
        self.v_t1 = self.m_speed0 + (self.m_accel_s * self.m_accel_s)/(2*self.m_jerk)

        self.s_t2 =  self.v_t1* self.T12 + self.m_accel_s * self.T12 * self.T12 / 2
        self.v_t2 =  self.m_speed - (self.m_accel_s * self.m_accel_s) /(2*self.m_jerk)

        self.s_t3 =  (self.v_t2 + (self.m_accel_s * self.m_accel_s)/(3*self.m_jerk))*(self.m_accel_s/self.m_jerk)
        self.v_t3 =  self.v_t2 + self.m_accel_s * self.T13 - self.m_jerk * self.T13 * self.T13 /2

        self.s_t123 = self.s_t1 + self.s_t2 + self.s_t3

        if self.s_t123 > half_dist:
            # Reaching max speed gives too large distance
            # Cut linear acceleration period
            lc = half_dist - self.s_t1
            if self.log:
                print('Too long distance {} > {}'.format(self.s_t123,half_dist))

            rr = self.v_t1*self.m_accel_s/self.m_jerk + (math.pow(self.m_accel_s,3) / (3* math.pow(self.m_jerk,2))) -lc
            rr1 = (self.m_speed0 + (self.m_accel_s*self.m_accel_s)/(6*self.m_jerk) )*(self.m_accel_s/self.m_jerk)
            rr1_2 = self.m_speed0 + (self.m_accel_s*self.m_accel_s)/(2*self.m_jerk)
            rr2 = (self.v_t1 + (self.m_accel_s*self.m_accel_s)/(3*self.m_jerk) )*(self.m_accel_s/self.m_jerk)

            coeff = [self.m_accel_s/2,
                     self.v_t1 + self.m_accel_s*self.m_accel_s/self.m_jerk,
                     rr]
            roots = np.roots(coeff)
            self.T12 = roots[1]

            self.s_t2 =  self.v_t1* self.T12 + self.m_accel_s * self.T12 * self.T12 / 2
            self.v_t2 =  self.v_t1 + self.m_accel_s * self.T12
            self.s_t3 =  (self.v_t2 + (self.m_accel_s * self.m_accel_s)/(3*self.m_jerk))*(self.m_accel_s/self.m_jerk)
            self.v_t3 =  self.v_t2 + self.m_accel_s * self.T13 - self.m_jerk * self.T13 *self.T13 /2
            self.s_t123 = self.s_t1 + self.s_t2 + self.s_t3

        if self.v_t3 > self.m_speed:
            if self.log:
                print('Too big speed {}>{}'.format(self.v_t3 , self.m_speed))
            # We are within distance limit, but we reach too big end speed
            self.T12  = (self.m_speed - self.v_t1 + self.m_jerk * self.T13 *self.T13 /2 - self.m_accel_s * self.T13)/ self.m_accel_s
            if self.T12 <0:
                self.T12 = 0
            self.s_t2 = self.v_t1 * self.T12 + self.m_accel_s * self.T12 * self.T12 / 2
            self.v_t2 = self.v_t1 + self.m_accel_s * self.T12
            self.s_t3 = (self.v_t2 + (self.m_accel_s * self.m_accel_s) / (3 * self.m_jerk)) * (self.m_accel_s / self.m_jerk)
            self.s_t123 = self.s_t1 + self.s_t2 + self.s_t3

        self.T1 = self.T11 + self.T12 + self.T13
        self.S1 = self.s_t123
 
        self.S2 = 2 * (half_dist - self.s_t123)
        self.T2 = self.S2 / self.m_speed
        self.T3 = self.T1
        self.S3 = self.S1

        if self.test_case:
            self.test_result = '\t{} /* T11 */, {} /* T12 */, {} /* T13 */,  {} /* T2 */, {} /* speed0 */, {} /* speed */, {} /* accel */, {} /* jerk */'.format(self.T11,self.T12,self.T13,self.T2,self.m_speed0,self.m_speed,self.m_accel_s,self.m_jerk)
            print('Test case\n')
            print('{\n\t{')
            print(self.test_set)
            print('\t},\n\t{')
            print(self.test_result)
            print('\t},\n}')

        self.T = self.T1 + self.T2 + self.T3
        if self.log:
            print('T11={} T12={} T13={}  T1={} half_dist/s1 = {}/{}  T2={}'.format(self.T11,self.T12,self.T13,self.T1 ,half_dist,self.s_t123,self.T2))
            print('TS={}/{}'.format(self.m_speed, self.m_speed_ini))

    def calc_rev_time(self,zdist):
        org_dist = zdist
        if self.m_dist < zdist:
            result = float('nan')
        elif zdist < self.S1:
            # First acceleration phase
            if zdist < self.s_t1:
                # T11 phase
                coeff = [self.m_jerk / 6, 0, self.m_speed0, -zdist]
                roots = np.roots(coeff)
                result = roots[2].real
            elif zdist < self.s_t1 + self.s_t2:
                # T12 phase
                zdist = zdist - self.s_t1

                coeff = [self.m_accel_s / 2, self.v_t1 , -zdist ]
                roots = np.roots(coeff)
                result = roots[1].real +  self.T11

            else:
                # T13 phase
                zdist = zdist - self.s_t1 - self.s_t2
                coeff = [-self.m_jerk / 6, self.m_accel_s / 2, self.v_t2, -zdist]
                roots = np.roots(coeff)
                result = roots[2].real + self.T11 + self.T12

        elif zdist < self.S1 + self.S2:
            # Constant speed phase
            zdist = zdist - self.S1
            result =  zdist/ self.m_speed + self.T1

        else:
            # Decceleration phase
            zdist = zdist - self.S1 - self.S2
            if zdist <  self.s_t3:
                # T31 phase
                coeff = [-self.m_jerk / 6, 0, self.v_t3, -zdist]
                roots = np.roots(coeff)
                result = roots[2].real + self.T1 + self.T2

            elif zdist < self.s_t3 + self.s_t2:
                # T32 phase
                zdist = zdist - self.s_t3
                coeff = [-self.m_accel_s / 2, self.v_t2, -zdist ]
                roots = np.roots(coeff)
                result = roots[1].real + self.T13 + self.T1 + self.T2
            else:
                # T33 phase
                zdist = zdist - self.s_t3 - self.s_t2
                coeff = [self.m_jerk / 6, -self.m_accel_s / 2, self.v_t1, -zdist]
                roots = np.roots(coeff)
                result = roots[2].real + self.T13 + self.T12 + self.T1 + self.T2

        if self.log:
            print('Dist={} Time={}'.format(org_dist,result))
        return result



    def one_step(self):
            if self.c_tick < self.T1:
                if self.c_tick < self.T11:
                    self.c_jerk = self.m_jerk
                elif self.c_tick < self.T11+self.T12:
                    self.c_jerk  = 0
                    self.c_accel = self.m_accel_s
                else:
                    self.c_jerk = -self.m_jerk

                self.c_accel = self.c_accel + self.c_jerk * self.m_tick
                self.c_speed = self.c_speed + self.c_accel * self.m_tick

            elif self.c_tick < self.T2 + self.T1:
                self.c_speed = self.m_speed
                self.c_jerk  = 0
            else:
                if self.c_tick < self.T2 + self.T1+ self.T11:
                    self.c_jerk = -self.m_jerk
                elif self.c_tick < self.T2 + self.T1 + self.T11+self.T12:
                    self.c_jerk = 0
                    self.c_accel = -self.m_accel_s
                else:
                    self.c_jerk = +self.m_jerk

                self.c_accel = self.c_accel + self.c_jerk * self.m_tick
                self.c_speed = self.c_speed + self.c_accel * self.m_tick

            self.c_dist = self.c_dist + self.c_speed * self.m_tick

    def  add_log(self):
        self.t_jerk.append(self.c_jerk)
        self.t_speed.append(self.c_speed)
        self.t_accel.append(self.c_accel)
        self.t_time.append(self.c_tick_total)
        self.t_dist.append(self.c_dist)

    def advance(self, idle=0):
        self.c_idx = self.c_idx + 1
        self.c_tick_total = self.c_tick_total + self.m_tick
        if idle == 0:
            self.c_tick = self.c_tick + self.m_tick

    def execute(self):
        self.calc_phase()

        if self.log:
            print('Total Time={}'.format(self.T))

        while self.m_dist >= self.c_dist:
            self.one_step()
            self.add_log()
            self.advance()

    def execute_seq(self,delay_time,total_time):
        while self.c_tick_total < delay_time:
            self.add_log()
            self.advance(idle =1)

        while self.m_dist >= self.c_dist:
            if self.c_tick_total > total_time:
                break
            self.one_step()
            self.add_log()
            self.advance()


        while self.c_tick_total < total_time:
            self.add_log()
            self.advance(idle =1)

    def plot(self,plt):
        # Plot the data
        plt.subplot(4, 1, 1)
        plt.plot(self.t_time, self.t_dist,label='distance')
        plt.legend(loc='upper right')

        plt.subplot(4, 1, 2)
        plt.plot(self.t_time, self.t_speed, label='speed')
        plt.legend(loc='upper right')

        plt.subplot(4, 1, 3)
        plt.plot(self.t_time, self.t_accel, label='acceleration')
        plt.legend(loc='upper right')

        plt.subplot(4, 1, 4)
        plt.plot(self.t_time, self.t_jerk, label='jerk')
        plt.legend(loc='upper right')

        # Show the plot
        plt.subplots_adjust(hspace=0.4)





"""
mp_x = motion_profile(1000,1000,10000,50)
mp_y = motion_profile(2000,2000,50000,50)

mp_path = motion_profile(3000,3000,25000,50)


p    = plt

test = motion_test()
test.trajectory()
test.path_plot(p)

p.show()
"""

'''

p = plt


move_dist = 3
move_z = -10
move_z_safe = -2
tick = 0.000001

mp_move = motion_profile(1000, 10000, 100000, 10)
mp_z = motion_profile(500, 5000, 50000, 10)

l_move = tick_calc(move_dist, mp_move, tick, log=1)
l_move.calc_phase()
l_z = tick_calc(-move_z, mp_z, tick, log=1)
l_z.calc_phase()

# time bugdet for Z blending - half of the XY move

t_budget = l_move.T1 + l_move.T2/2
t_z_all = l_z.T1 + l_z.T2 + l_z.T3
t_z_safe = l_z.calc_rev_time(-move_z - -move_z_safe)

t_z_delta = t_z_all - t_z_safe

if t_z_delta < t_budget:
    t_z_delay = t_z_safe
else:
    t_z_delay = t_z_all - t_budget

l_move.execute_seq(t_z_delay, t_z_delay + t_budget)
l_z.execute_seq(0, t_z_delay + t_budget)


for ii in range(0,len(l_z.t_dist)):
    l_z.t_dist[ii] = l_z.t_dist[ii]- -move_z

p.plot(l_move.t_dist, l_z.t_dist , label='x_z blending')
p.plot([0, move_dist/2 ], [move_z_safe,move_z_safe], 'k-', lw=2)

end_z_marker = l_move.t_dist[ int(t_z_all /tick)]


p.plot( [end_z_marker, end_z_marker ],[0,move_z], 'g.', lw=1)


p.show()

'''

tt = 10
acc = 100000
jerk = 10
vel0 = 10
vel = vel0 + tt*acc

#jerk = acc*acc/(tt*acc)

jerk = 20000
acc  = acc *2

move_dist = 2* (vel0 * tt + acc * tt * tt/2)
print(move_dist/2)
print(jerk)

mp = motion_profile(vel, acc, jerk, vel0)
move = tick_calc(move_dist, mp, 0.0001 , log=1)
move.execute()



p = plt

move.plot(p)

p.interactive(False)
p.show(block=True)

print('Done')


import matplotlib.pyplot as plt
import array as arr
import math
import numpy as np


class tick:
    def __init__(self):
        self.plt       = plt
        #self.m_dist    = 20000.0
        #self.m_speed   = 1500.0
        #self.m_speed0  = 500.0
        #self.m_accel_s = 1000.0
        #self.m_jerk    = 500.0

        self.m_dist    = 10.0
        self.m_speed   = 50.0
        self.m_speed0  =  5.0
        self.m_accel_s = 50.0
        self.m_jerk    = 50.0

        self.m_tick    = 0.00001
        self.c_speed   = self.m_speed0
        self.c_accel   = 0
        self.c_jerk    = 0
        self.c_dist    = 0
        self.c_tick    = 0
        self.c_idx     = 0

        self.test_set = '\t{}/*dist*/, {} /* speed0 */, {} /* speed */, {} /* accel */, {} /* jerk */'.format(self.m_dist, self.m_speed0, self.m_speed, self.m_accel_s,self.m_jerk)

        self.t_dist   = arr.array('f')
        self.t_speed  = arr.array('f')
        self.t_accel  = arr.array('f')
        self.t_jerk   = arr.array('f')
        self.t_time   = arr.array('f')

    def calc_phase(self):
        hd = self.m_dist / 2

        h_cc = (2*self.m_speed0 + self.m_accel_s * self.m_accel_s/self.m_jerk)*self.m_accel_s/self.m_jerk

        if h_cc > hd:
            #Acceleration needs to be lowered - even ideal S curve gives too big distance
            old_acc = self.m_accel_s
            coeff = [1/(self.m_jerk*self.m_jerk),(2*self.m_speed0/self.m_jerk),0,-hd]
            roots = np.roots(coeff)
            self.m_accel_s = roots[2]
            print('Changing acceleration from {} to {} (distance limit)'.format(old_acc,self.m_accel_s))

        h_v  = self.m_speed0 + self.m_accel_s * self.m_accel_s/self.m_jerk

        if h_v > self.m_speed:
            #Given acceleration makes too big speed - even for ideal S curve
            old_acc = self.m_accel_s
            old_speed = self.m_speed
            self.m_accel_s = math.sqrt((self.m_speed - self.m_speed0)*self.m_jerk)
            self.m_speed   = self.m_speed0 + self.m_accel_s * self.m_accel_s/self.m_jerk
            print('Changing acceleration {}->{} and speed {}->{} (max speed limit)'.format(old_acc, self.m_accel_s,old_speed,self.m_speed ))

        self.T11 = self.m_accel_s / self.m_jerk
        self.T13 = self.T11
        self.T12 = (self.m_speed - self.m_speed0 - (self.m_accel_s * self.m_accel_s)/self.m_jerk)/self.m_accel_s
        if self.T12 < 0:
            self.T12 = 0

        self.s_t1 = (self.m_speed0 + (self.m_accel_s * self.m_accel_s)/(6*self.m_jerk))*(self.m_accel_s/self.m_jerk)
        self.v_t1 = self.m_speed0 + (self.m_accel_s * self.m_accel_s)/(2*self.m_jerk)

        self.s_t2 =  self.v_t1* self.T12 + self.m_accel_s * self.T12 * self.T12 / 2
        self.s_v2 =  self.m_speed - (self.m_accel_s * self.m_accel_s) /(2*self.m_jerk)

        self.s_t3 =  (self.s_v2 + (self.m_accel_s * self.m_accel_s)/(3*self.m_jerk))*(self.m_accel_s/self.m_jerk)
        self.v_t3 =  self.s_v2 + self.m_accel_s * self.T13 - self.m_jerk * self.T13 * self.T13 /2

        self.s_t123 = self.s_t1 + self.s_t2 + self.s_t3

        if self.s_t123 > hd:
            # Reaching max speed gives too large distance
            # Cut linear acceleration period
            lc = hd - self.s_t1
            print('Too long distance {} > {}'.format(self.s_t123,hd))
            coeff = [self.m_accel_s/2, self.v_t1 + self.m_accel_s*self.T11,self.T11* self.v_t1+self.m_accel_s*self.T11*self.T11/2-self.m_jerk*self.T11*self.T11*self.T11/6-lc]
            roots = np.roots(coeff)
            self.T12 = roots[1]

            self.s_t2 =  self.v_t1* self.T12 + self.m_accel_s * self.T12 * self.T12 / 2
            self.s_v2 =  self.v_t1 + self.m_accel_s * self.T12
            self.s_t3 =  (self.s_v2 + (self.m_accel_s * self.m_accel_s)/(3*self.m_jerk))*(self.m_accel_s/self.m_jerk)
            self.v_t3 =  self.s_v2 + self.m_accel_s * self.T13 - self.m_jerk * self.T13 *self.T13 /2
            self.s_t123 = self.s_t1 + self.s_t2 + self.s_t3

        if self.v_t3 > self.m_speed:
            print('Too big speed {}>{}'.format(self.v_t3 , self.m_speed))
            # We are within distance limit, but we reach too big end speed
            self.T12  = (self.m_speed  -   self.v_t1 + self.m_jerk * self.T13 *self.T13 /2 - self.m_accel_s * self.T13)/  self.m_accel_s
            if self.T12 <0:
                self.T12 = 0
            self.s_t2 = self.v_t1 * self.T12 + self.m_accel_s * self.T12 * self.T12 / 2
            self.s_v2 = self.v_t1 + self.m_accel_s * self.T12
            self.s_t3 = (self.s_v2 + (self.m_accel_s * self.m_accel_s) / (3 * self.m_jerk)) * (self.m_accel_s / self.m_jerk)
            self.s_t123 = self.s_t1 + self.s_t2 + self.s_t3

        self.T1 = self.T11 + self.T12 + self.T13
        self.T2 =  2 * (hd -  self.s_t123) / self.m_speed
        self.test_result = '\t{} /* T11 */, {} /* T12 */, {} /* T13 */,  {} /* T2 */, {} /* speed0 */, {} /* speed */, {} /* accel */, {} /* jerk */'.format(self.T11,self.T12,self.T13,self.T2,self.m_speed0,self.m_speed,self.m_accel_s,self.m_jerk)
        print('Test case\n')
        print('{\n\t{')
        print(self.test_set)
        print('\t},\n\t{')
        print(self.test_result)
        print('\t},\n}')

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


    def execute(self):
        self.calc_phase()

        while self.m_dist >= self.c_dist:
            self.one_step()
            self.add_log()
            self.advance()

    def  add_log(self):
        self.t_jerk.append(self.c_jerk)
        self.t_speed.append(self.c_speed)
        self.t_accel.append(self.c_accel)
        self.t_time.append(self.c_tick)
        self.t_dist.append(self.c_dist)

    def advance(self):
        self.c_idx = self.c_idx + 1
        self.c_tick = self.c_tick + self.m_tick

    def plot(self):
        # Plot the data
        self.plt.subplot(4, 1, 1)
        self.plt.plot(self.t_time, self.t_dist, label='distance')
        self.plt.legend()

        self.plt.subplot(4, 1, 2)
        self.plt.plot(self.t_time, self.t_speed, label='speed')
        self.plt.legend()

        self.plt.subplot(4, 1, 3)
        self.plt.plot(self.t_time, self.t_accel, label='acceleration')
        self.plt.legend()

        self.plt.subplot(4, 1, 4)
        self.plt.plot(self.t_time, self.t_jerk, label='jerk')
        self.plt.legend()

        # Show the plot
        self.plt.subplots_adjust(hspace=0.4)
        self.plt.show()

tt = tick()
tt.execute()
tt.plot()
print('Done')


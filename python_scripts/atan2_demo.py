import numpy as np
import matplotlib.pyplot as plt

piQ30 = 843314857

def atan2(y, x):
    if x > 0:
        if y >= 0: # this ensures that |y+x| >= |y-x| and hence |a| <= 1
            a = ((y<<27) - (x<<27)) // (y + x)
            return q30_atan_normalized( a )
        else: # this ensures that |y-x| >= |y+x| and hence |a| <= 1
            a = ((y<<27) + (x<<27)) // (y - x)
            return -q30_atan_normalized( a )
    if x < 0:
        if y >= 0:
            a = ((y<<27) + (x<<27)) // (y - x)
            return piQ30 - q30_atan_normalized( a )
        else:
            a = ((y<<27) - (x<<27)) // (y + x)
            return q30_atan_normalized( a ) - piQ30
    if y > 0: return piQ30//2
    if y < 0: return -piQ30//2
    return Q30(0.0)


def q30_atan_normalized(a):
    a2 = (a*a)>>28
    # at = Q30(0.25)
    at = 210828714
    k = a
    # at += Q30(0.63611725)*k>>28
    at += (536447127*k)>>28
    k = (k*a2)>>28
    # at += (-Q30(0.817719)*k)>>28
    at += (-689594581*k)>>28
    k = (k*a2)>>28
    # at += (Q30(1.48697)*k)>>28
    at += (1253983892*k)>>28
    k = (k*a2)>>28
    # at += (-Q30(1.57588)*k)>>28
    at += (-1328963016*k)>>28
    return at


def Q30(x):
    return round(x*2**28)


Q30_a = lambda x: (x*2**28).round().astype('int32')
lim = 249*np.pi/500

a = np.linspace(-lim, lim, 1000)
a = np.linspace(3*np.pi/4, 5*np.pi/4, 1000)
s = np.sin(a)
c = np.cos(a)
# t = np.linspace(1.0, 0.0, 64)
# x = np.arctan(c)
aQ30 = Q30_a(a)
sQ30 = Q30_a(s)
cQ30 = Q30_a(c)

plt.subplot(211)
plt.title("Biểu đồ  sai số  hàm arctan xấp xỉ")
plt.plot((s/c), np.arctan2(s, c))
# plt.plot((s/c), np.vectorize(atan2)(sQ30, cQ30)/2**28)
plt.ylabel("Arctan (radians)")
plt.subplot(212)
plt.plot((s/c), np.arctan2(s, c) - np.vectorize(atan2)(sQ30, cQ30)/2**28)
plt.xlabel("Giá trị tan (Q30)")
plt.ylabel("Sai số (radians)")
plt.show()

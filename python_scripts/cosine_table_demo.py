"""
Test hàm tính xấp xỉ góc từ cosine (arccos) và sai số
so với hàm trong thư viện chuẩn.
"""

import numpy as np
import matplotlib.pyplot as plt

piQ3_28 = 843314857

x = np.linspace(0, np.pi/2, 64)
c = np.cos(x)
# c = np.linspace(1.0, 0.0, 64)
# x = np.arccos(c)
xQ3_28 = (x*2**28).round().astype('int32')
cQ3_28 = (c*2**28).round().astype('int32')

def acos_lookup(c):
    s = 0
    if c < 0:
        c = -c
        s = 1
    i = 0
    j = 63
    while i+1 != j:
        k = (i+j)>>1
        if (c > cQ3_28[k]):
            j = k
        else:
            i = k
    angle = xQ3_28[i]
    interpolated = (c - cQ3_28[i])*(xQ3_28[i] - xQ3_28[j])
    interpolated = interpolated // (cQ3_28[i] - cQ3_28[j])
    angle += interpolated
    if s:
        angle = piQ3_28 - angle;
    return angle

c2 = np.linspace(-0.5, 0.5, 1000)
c2Q3_28 = (c2*2**28).round().astype('int32')
x2 = np.arccos(c2)
plt.plot(c2, x2 - np.vectorize(acos_lookup)(c2Q3_28)/2**28)
plt.xlabel("Giá trị cosin (Q3_28)")
plt.ylabel("Sai số (radians)")
plt.title("Biểu đồ  sai số  arccos tra bảng")
plt.show()

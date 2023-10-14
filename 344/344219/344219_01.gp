a(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m, eulerphi(i)*eulerphi(j)*eulerphi(k)*eulerphi(l)*eulerphi(m)/eulerphi(lcm([i, j, k, l, m])))))));

M=500;
for(n=1, M, write("/Users/xxx/Desktop/b344219.txt", n, " ", a(n)))
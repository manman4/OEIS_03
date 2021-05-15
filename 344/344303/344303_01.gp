M=28;

a160897(n) = sumdiv(n, d, moebius(n/d)*d^7)/eulerphi(n);
a(n) = sumdiv(n, d, a160897(d));

for(n=1, M, print1(a160897(n), ", "));
for(n=1, M, print1(a(n), ", "));
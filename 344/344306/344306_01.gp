M=20;

a160957(n) = sumdiv(n, d, moebius(n/d)*d^10)/eulerphi(n);
a(n) = sumdiv(n, d, a160957(d));

for(n=1, M, print1(a160957(n), ", "));
for(n=1, M, print1(a(n), ", "));
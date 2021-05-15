M=31;

a160895(n) = sumdiv(n, d, moebius(n/d)*d^6)/eulerphi(n);
a(n) = sumdiv(n, d, a160895(d));

for(n=1, M, print1(a160895(n), ", "));
for(n=1, M, print1(a(n), ", "));
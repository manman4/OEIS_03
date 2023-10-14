M=26;

a160908(n) = sumdiv(n, d, moebius(n/d)*d^8)/eulerphi(n);
a(n) = sumdiv(n, d, a160908(d));

for(n=1, M, print1(a160908(n), ", "));
for(n=1, M, print1(a(n), ", "));
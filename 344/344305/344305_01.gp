M=24;

a160953(n) = sumdiv(n, d, moebius(n/d)*d^9)/eulerphi(n);
a(n) = sumdiv(n, d, a160953(d));

for(n=1, M, print1(a160953(n), ", "));
for(n=1, M, print1(a(n), ", "));
M=20;

a(n) = sumdiv(n, d, eulerphi(n/d)*d^d);
for(n=1, M, print1(a(n), ", "));

M=20;

a(n) = sumdiv(n, d, eulerphi(d)*d^(n-2));
for(n=1, M, print1(a(n), ", "));
M=20;

a(n) = sumdiv(n, d, eulerphi(d^(n-1)));
for(n=1, M, print1(a(n), ", "));

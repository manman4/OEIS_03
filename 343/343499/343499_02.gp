M=34;

a(n) = sumdiv(n, d, eulerphi(n/d)*d^5);
for(n=1, M, print1(a(n),", ")); 
M=40;

a(n) = sumdiv(n, d, eulerphi(d)^(n-d+1));
for(n=1, M, print1(a(n), ", "));
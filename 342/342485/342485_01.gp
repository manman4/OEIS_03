M=50;

a(n) = sumdiv(n, d, eulerphi(d)^(d+1));
for(n=1, M, print1(a(n), ", "));
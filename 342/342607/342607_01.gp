M=40;

a(n) = sumdiv(n, d, eulerphi(d)^(n-d));
for(n=1, M, print1(a(n), ", "));

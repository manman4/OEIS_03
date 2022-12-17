M=79;

a(n) = sumdiv(n, d, eulerphi(7*d))/6;
for(n=1, M, print1(a(n), ", "));
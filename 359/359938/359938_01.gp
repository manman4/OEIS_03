M=90;

a(n) = sumdiv(n, d, issquare(d-1));
for(n=1, M, print1(a(n), ", "));
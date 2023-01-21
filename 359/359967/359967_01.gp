M=86;

a(n) = sumdiv(n, d, issquare(d+1)*d);
for(n=1, M, print1(a(n), ", "));
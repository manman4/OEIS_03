M=56;

a(n) = n*sumdiv(n, d, n^omega(d)/d);
for(n=1, M, print1(a(n), ", "));
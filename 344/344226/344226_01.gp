M=70;

a(n) = sumdiv(n, d, n^omega(d)/d);
for(n=1, M, print1(a(n), ", "));
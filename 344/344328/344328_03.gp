M=77;

a(n) = sumdiv(n, d, 5^omega(d));
for(n=1, M, print1(a(n), ", "));
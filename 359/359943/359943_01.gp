M=86;

a(n) = sumdiv(n, d, ispower(d-1, 3)*d);
for(n=1, M, print1(a(n), ", "));
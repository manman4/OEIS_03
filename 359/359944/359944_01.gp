M=91;

a(n) = sumdiv(n, d, ispower(d-1, 3));
for(n=1, M, print1(a(n), ", "));
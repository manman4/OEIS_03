M=67;

a(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, n/lcm(lcm(i, j), k))));
for(n=1, M, print1(a(n), ", "));
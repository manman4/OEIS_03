M=61;

a(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, n/lcm(lcm(lcm(i, j), k), l)))));
for(n=1, M, print1(a(n), ", "));
M=51;

a(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, i*j*k/lcm([i, j, k]))));
for(n=1, M, print1(a(n), ", "));

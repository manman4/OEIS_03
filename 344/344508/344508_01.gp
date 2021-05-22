M=42;

a(n) = sum(k=1, n, k*lcm(k, n));
for(n=1, M, print1(a(n), ", "));
M=19;

a(n) = (-1)^n * n! * sum(k=0, n, (-(k+1))^k/(k+1)!);
for(n=0, M, print1(a(n), ", "))

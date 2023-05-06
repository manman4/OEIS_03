M=13;

a(n) = polcoef(sum(k=0, n, (n*k*x)^k / (1 + x + x*O(x^n))^(k+1)), n);
for(n=0, M, print1(a(n),", "))

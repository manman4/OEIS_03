M=27;

a(n) = polcoef(sum(k=1, n, (1/(1 - x^k)^n + x*O(x^n)) - 1 ), n);
for(n=1, M, print1(a(n),", "));

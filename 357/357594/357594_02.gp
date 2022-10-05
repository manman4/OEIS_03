M=22;

a(n) = 2*sum(k=0, n\2, (-1)^k*(1-4^k)*abs(stirling(n, 2*k, 1))*bernfrac(2*k));
for(n=0, M, print1(a(n), ", "));
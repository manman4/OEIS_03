M=25;

a(n) = sum(k=1, n\2, ((-4)^k-(-16)^k)*bernfrac(2*k)/(2*k)*binomial(n, 2*k));
for(n=1, M, print1(a(n), ", "));
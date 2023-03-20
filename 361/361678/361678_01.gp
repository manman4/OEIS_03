M=23;

a(n) = sum(k=0, n\4, (4*k)!/k!^4*binomial(4*k, k)*binomial(n, 4*k));
for(n=0, M, print1(a(n), ", "));

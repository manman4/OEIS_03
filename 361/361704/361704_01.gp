M=25;

a(n) = sum(k=0, n\6, (4*k)!/k!^4*binomial(6*k, 4*k)*binomial(n, 6*k));
for(n=0, M, print1(a(n), ", "));

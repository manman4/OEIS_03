M=27;

a(n) = sum(k=0, n\8, (4*k)!/k!^4*binomial(8*k, 4*k)*binomial(n, 8*k));
for(n=0, M, print1(a(n), ", "));

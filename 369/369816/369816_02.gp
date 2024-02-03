a(n) = sum(k=0, n\7, ((n-2*k)%5==0)*binomial((n-2*k)/5, k));
for(n=0, 78, print1(a(n), ", "));

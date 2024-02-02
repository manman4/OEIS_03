a(n) = sum(k=0, n\7, ((n-4*k)%3==0)*binomial((n-4*k)/3, k));
for(n=0, 71, print1(a(n), ", "));

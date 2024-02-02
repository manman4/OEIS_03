a(n) = sum(k=0, n\7, ((n-5*k)%2==0)*binomial((n-5*k)/2, k));
for(n=0, 67, print1(a(n), ", "));

a(n) = sum(k=0, n\5, ((n-3*k)%2==0)*binomial((n-3*k)/2, k)^2);
for(n=0, 45, print1(a(n),", "))




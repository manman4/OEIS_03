a(n) = sum(k=0, n\5, ((n-2*k)%3==0)*binomial((n-2*k)/3, k)^2);
for(n=0, 49, print1(a(n),", "))




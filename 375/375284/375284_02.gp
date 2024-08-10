\\ a(n) = Sum_{k=0..floor(n/5)} binomial(2*n-8*k,2*k).
a(n) = sum(k=0, n\5, binomial(2*n-8*k, 2*k));
for(n=0, 38, print1(a(n),", ")) 

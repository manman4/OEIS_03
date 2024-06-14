\\ a(n) = Sum_{k=0..floor(2*n/9)} binomial(2*n-8*k,k).
a(n) = sum(k=0, 2*n\9, binomial(2*n-8*k,k));
for(n=0, 44, print1(a(n),", "))

b(n) = my(x='x+O('x^(n+10))); polcoef((1+x^8)/(1-x-x^9), n);
for(n=1, 100, print1(a(n)-b(2*n-1),", "))


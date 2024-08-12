\\ a(n) = Sum_{k=0..floor(3*n/5)} binomial(3*n-3*k,2*k).
a(n) = sum(k=0, 3*n\5, binomial(3*n-3*k, 2*k));                                                                             
for(n=0, 26, print1(a(n),", ")) 

a116090(n) = sum(k=0, n\2, binomial(3*k, n-2*k));
for(n=0, 100, print1(a(n)-a116090(2*n),", "))


   
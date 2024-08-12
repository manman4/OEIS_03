\\ a(n) = Sum_{k=0..floor(n/2)} binomial(3*k+1,n-2*k).
a(n) = sum(k=0, n\2, binomial(3*k+1, n-2*k));                                                                                 
for(n=0, 36, print1(a(n),", ")) 

\\ a(n) = Sum_{k=0..floor(n/2)} C(3*k, n-2*k).
a116090(n) = sum(k=0, n\2, binomial(3*k, n-2*k));
b(n) = if(n==0, 1, a116090(n) + a116090(n-1));
for(n=0, 100, print1(a(n) - b(n),", "))   
\\ a(n) = Sum_{k=0..floor(n/2)} binomial(4*k+1,n-2*k).
a(n) = sum(k=0, n\2, binomial(4*k+1, n-2*k));                                                                                
for(n=0, 33, print1(a(n),", ")) 

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(4*k,n-2*k).
a375314(n) = sum(k=0, n\2, binomial(4*k, n-2*k));
b(n) = if(n==0, 1, a375314(n) + a375314(n-1));
for(n=0, 100, print1(a(n) - b(n),", "))   
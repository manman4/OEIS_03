\\ a(n) = Sum_{k=0..floor(n/3)} binomial(4*k+1,n-3*k).
a(n) = sum(k=0, n\3, binomial(4*k+1, n-3*k));                                                                             
for(n=0, 37, print1(a(n),", ")) 

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(4*k,n-3*k).
a375318(n) = sum(k=0, n\3, binomial(4*k, n-3*k));
b(n) = if(n==0, 1, a375318(n) + a375318(n-1));
for(n=0, 100, print1(a(n) - b(n),", "))   
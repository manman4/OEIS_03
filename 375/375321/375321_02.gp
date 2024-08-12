\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*k+2,n-3*k).
a(n) = sum(k=0, n\3, binomial(3*k+2, n-3*k));                                                                             
for(n=0, 39, print1(a(n),", ")) 

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*k+1,n-3*k).
a375319(n) = sum(k=0, n\3, binomial(3*k+1, n-3*k));
b(n) = if(n==0, 1, a375319(n) + a375319(n-1));
for(n=0, 100, print1(a(n) - b(n),", "))   
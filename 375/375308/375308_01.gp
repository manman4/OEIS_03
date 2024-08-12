\\ a(n) = Sum_{k=0..floor(2*n/3)} binomial(4*n-4*k,2*k).
a(n) = sum(k=0, 2*n\3, binomial(4*n-4*k, 2*k));                                                                                 
for(n=0, 24, print1(a(n),", ")) 

a375279(n) = sum(k=0, n\3, binomial(2*n-4*k, 2*k));
for(n=0, 100, print1(a(n)-a375279(2*n),", "))

a375314(n) = sum(k=0, n\2, binomial(4*k, n-2*k));
for(n=0, 100, print1(a(n)-a375314(2*n),", "))
   
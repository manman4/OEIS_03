M=43;

\\ a(n) = Sum_{k=0..floor(n/3)} (k+1) * binomial(2*k,2*n-6*k).
b(n) = sum(k=0, n\3, (k+1)*binomial(2*k,2*n-6*k));
for(n=0, M, print1(b(n),", "))     

\\ signature (0,0,4,4,0,-6,-4,-6,4,-4,-4,3,4,-6,4,-1).
a(n) = if(n<16, b(n), 4*a(n-3)+4*a(n-4)-6*a(n-6)-4*a(n-7)-6*a(n-8)+4*a(n-9)-4*a(n-10)-4*a(n-11)+3*a(n-12)+4*a(n-13)-6*a(n-14)+4*a(n-15)-a(n-16));
for(n=0, 29, print1(a(n)-b(n),", "))





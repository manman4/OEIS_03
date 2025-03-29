M=36;

\\ a(n) = Sum_{k=0..floor(n/2)} (k+1) * binomial(2*k,2*n-4*k).
b(n) = sum(k=0, n\2, (k+1)*binomial(2*k,2*n-4*k));
for(n=0, M, print1(b(n),", "))     

\\ signature (0,4,4,-6,-4,-2,-4,-5,8,-6,4,-1).
a(n) = if(n<12, b(n), 4*a(n-2)+4*a(n-3)-6*a(n-4)-4*a(n-5)-2*a(n-6)-4*a(n-7)-5*a(n-8)+8*a(n-9)-6*a(n-10)+4*a(n-11)-a(n-12));
for(n=0, 25, print1(a(n)-b(n),", "))





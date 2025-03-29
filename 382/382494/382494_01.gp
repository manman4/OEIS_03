M=33;

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+2,2) * binomial(2*k,2*n-4*k).
b(n) = sum(k=0, n\2, binomial(k+2,2)*binomial(2*k,2*n-4*k));
for(n=0, M, print1(b(n),", "))     

\\ signature (0,6,6,-15,-18,5,12,-3,32,12,-6,-4,18,-33,26,-15,6,-1).
a(n) = if(n<18, b(n), 6*a(n-2)+6*a(n-3)-15*a(n-4)-18*a(n-5)+5*a(n-6)+12*a(n-7)-3*a(n-8)+32*a(n-9)+12*a(n-10)-6*a(n-11)-4*a(n-12)+18*a(n-13)-33*a(n-14)+26*a(n-15)-15*a(n-16)+6*a(n-17)-a(n-18));
for(n=0, 28, print1(a(n)-b(n),", "))







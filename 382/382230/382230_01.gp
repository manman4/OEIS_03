M=27;

\\ a(n) = Sum_{k=0..n} binomial(k+2,2) * binomial(2*k,2*n-2*k).
b(n) = sum(k=0, n, binomial(k+2, 2)*binomial(2*k, 2*n-2*k));
for(n=0, M, print1(b(n),", "))     

\\ signature (6,-9,2,-18,30,7,30,-18,2,-9,6,-1).
a(n) = if(n<12, b(n), 6*a(n-1)-9*a(n-2)+2*a(n-3)-18*a(n-4)+30*a(n-5)+7*a(n-6)+30*a(n-7)-18*a(n-8)+2*a(n-9)-9*a(n-10)+6*a(n-11)-a(n-12));
for(n=8, 25, print1(a(n)-b(n),", "))


M=20;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} (n-3*k)^k * binomial(3*k+1,n-3*k)/( (3*k+1)*k! ).
a(n) = n! * sum(k=0, n\3, (n-3*k)^k * binomial(3*k+1,n-3*k)/( (3*k+1)*k! ));
for(n=0, M, print1(a(n)", "))
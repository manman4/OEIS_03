M=17;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} (n-2*k)^k * (2*n-2*k)!/(2^k * k! * (n-2*k)!).
a(n) = (1/(n+1)) * sum(k=0, n\2, (n-2*k)^k * (2*n-2*k)!/(2^k * k! * (n-2*k)!));
for(n=0, M, print1(a(n),", "))   

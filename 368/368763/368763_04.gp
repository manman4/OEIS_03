a368574(n) = n! * sum(k=0, n, binomial(k+2,3) / k!);
for(n=0, 21, print1(a368574(n), ", "));
a(n) = n! + a368574(n);
for(n=0, 21, print1(a(n), ", "));  

a103519(n) = n! * sum(k=0, n, binomial(k+1,2) / k!);
for(n=1, 21, print1(a103519(n), ", "));
a(n) = n! + a103519(n);
for(n=0, 21, print1(a(n), ", "));  

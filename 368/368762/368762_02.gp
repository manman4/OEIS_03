\\ a(n) = n! * (1 + Sum_{k=0..n} binomial(k+1,2) / k!).
a(n) = n! * (1 + sum(k=0, n, binomial(k+1, 2) / k!));
for(n=0, 21, print1(a(n), ", "));  

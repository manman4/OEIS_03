a(n) = n! * sum(k=0, n\2, (-2)^k * binomial(n-k,k)/(n-k)!);
for(n=0, 23, print1(a(n),", "))        

a(n) = n! * sum(k=0, n\2, (-2)^k / (k! * (n-2*k)!));
for(n=0, 23, print1(a(n),", "))    
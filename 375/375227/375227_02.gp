\\ a(n) = n! * Sum_{k=0..floor(n/3)} binomial(n+2,n-3*k)/k!.
a(n) = n! * sum(k=0, n\3, binomial(n+2, n-3*k)/k!);                                                                            
for(n=0, 18, print1(a(n),", "))   
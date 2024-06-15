\\ a(n) = n! * Sum_{k=0..floor(3*n/7)} binomial(3*n-6*k,k)/(n-2*k)!.
a(n) = n! * sum(k=0, 3*n\7, binomial(3*n-6*k, k)/((n-2*k)!));
for(n=0, 21, print1(a(n),", ")) 
for(n=0, 100, print1((a(n)-1)%18,", ")) 

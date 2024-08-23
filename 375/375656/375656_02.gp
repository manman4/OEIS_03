\\ a(n) = n! * Sum_{k=0..floor(n/3)} ((n-3*k+6)/6)^k / (k! * (n-3*k)!).
a(n) = n! * sum(k=0, n\3, ((n-3*k+6)/6)^k / (k! * (n-3*k)!));
for(n=0, 23, print1(a(n), ", "))

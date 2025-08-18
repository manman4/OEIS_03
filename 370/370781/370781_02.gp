\\ a(n) = Sum_{k=0..floor(n/2)} A004987(k) * binomial(n,2*k). 
b(n) = (-9)^n*binomial(-1/3, n);
for(n=0, 22, print1(b(n),", "));
a(n) = sum(k=0, n\2, b(k)*binomial(n, 2*k));
for(n=0, 15, print1(a(n),", "));
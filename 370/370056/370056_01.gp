M=16;

a(n) = 2*(4*n+1)!/(3*n+2)!;
for(n=0, M, print1(a(n), ", "))

a069271(n) = binomial(4*n+1,n)*2/(3*n+2);
for(n=0, M, print1(a069271(n), ", "))
\\ a(n) = A000142(n) * A069271(n).
for(n=0, M, print1(a(n) - n! * a069271(n), ", "))
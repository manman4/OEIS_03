M=16;

a(n) = 3*(4*n+2)!/(3*n+3)!;
for(n=0, M, print1(a(n), ", "))

a006632(n) = 3*binomial(4*n-1,n-1)/(4*n-1);
for(n=1, M, print1(a006632(n), ", "))
\\ a(n) = A000142(n) * A006632(n+1).
for(n=0, M, print1(a(n) - n! * a006632(n+1), ", "))
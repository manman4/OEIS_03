M=25;

\\ a(n) = Sum_{k=0..n} binomial(k+2,2) * binomial(n+2*k-1,n-k).
b(n) = sum(k=0, n, binomial(k+2,2) * binomial(n+2*k-1,n-k));
for(n=0, M, print1(b(n),", "));

\\ a(n) = 12*a(n-1) - 57*a(n-2) + 139*a(n-3) - 195*a(n-4) + 174*a(n-5) - 102*a(n-6) + 39*a(n-7) - 9*a(n-8) + a(n-9) for n > 9.
\\ 以下はダメ
a(n) = if(n<9, b(n), 12*a(n-1) - 57*a(n-2) + 139*a(n-3) - 195*a(n-4) + 174*a(n-5) - 102*a(n-6) + 39*a(n-7) - 9*a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n),", "));
\\ 以下はOK
a(n) = if(n<10, b(n), 12*a(n-1) - 57*a(n-2) + 139*a(n-3) - 195*a(n-4) + 174*a(n-5) - 102*a(n-6) + 39*a(n-7) - 9*a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n),", "));



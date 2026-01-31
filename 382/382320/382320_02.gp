M=25;

a382321(n) = my(A=x +x*O(x^n)); for(i=1, n, A = x/(1-x +x*O(x^n)) + sum(k=1, n, subst(Ser(A)^2, x, x^k) ) ); polcoef(A, n);
\\ a(n) = Sum_{k=1..n-1} A382321(k) * A382321(n-k) for n>1. 
a(n) = if(n==1, 1, sum(i=1, n-1, a382321(i) * a382321(n-i)));
for(n=1, M, print1(a(n),", "));
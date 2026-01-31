M=25;

a382319(n) = my(A=x +x*O(x^n)); for(i=1, n, A = x/(1-x +x*O(x^n)) + sum(k=1, n, subst(Ser(A)^3, x, x^k) ) ); polcoef(A, n);
\\ a(n) = Sum_{i,j,k >= 1 and i + j + k = n} A382319(i) * A382319(j) * A382319(k) for n>1. 
a(n) = if(n==1, 1, sum(i=1, n-2, sum(j=1, n-1-i, a382319(i) * a382319(j) * a382319(n-i-j))));
for(n=1, M, print1(a(n),", "));
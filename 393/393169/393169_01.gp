\\ a(n) = 6^n * n! * binomial((n+1)/6,n)/(n+1).
a(n) = 6^n * n! * binomial((n+1)/6,n)/(n+1);
for(n=0, 40, print1(a(n),", ")); 

\\ a(6*n+5) = 0.
for(n=0, 5000, if(a(6*n+5)!=0, print1(n,", ")));  
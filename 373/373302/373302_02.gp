a(n, m) = my(x='x+O('x^(6*n+10))); polcoef(1/(1-x-x^6), 6*n-m); 
for(n=1, 25, print1(a(n, 2),", "))

\\ A099242(n) (6n+5)-th terms of expansion of 1/(1 - x - x^6).
for(n=1, 34, print1(a(n-1, -5) - a(n-2, -5),", "))
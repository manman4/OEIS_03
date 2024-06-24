a(n, m) = my(x='x+O('x^(6*n+10))); polcoef(1/(1-x^5-x^6), 6*n-m); 
for(n=1, 25, print1(a(n, 3),", "))

for(n=1, 34, print1(a(n+1, 4) - a(n, 4),", "))